#include "command.h"
#include "debug.h"
#include "parser.h"

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int shell_input_fd_;
static bool shell_interactive_;
static const char *input_string_ = NULL;
const char *program_argv_0;

static int shell(int input_fd, bool interactive);

static void handle_sig(int);
static void register_sighandler(int);

extern bool yydebug;

int main(int argc, const char **argv) {
    (void)argc;
    program_argv_0 = argv[0];

    yydebug = false;
    register_sighandler(SIGINT);

    int infile = STDIN_FILENO;
    bool interactive = isatty(STDIN_FILENO);

    for (const char **it = argv + 1; *it; it++) {
        if ((*it)[0] == '-') {
            // parse options
            switch ((*it)[1]) {
            case 'c':
                input_string_ = *(++it);
                interactive = false;
                break;
            default:
                errx(1, "unknown option");
            }
        } else {
            // first non-option is a script file
            if (input_string_ != NULL) {
                err(1, "cannot combine -c option with file argument");
            }
            if ((infile = open(*it, 0)) < 0) {
                err(1, "open file");
            }
            interactive = false;
            break;
        }
    }

    return shell(infile, interactive);
}

static void handle_sig(int sig) {
    //tracef("Signal %d: %s", sig, strsignal(sig));
    (void)sig;
}

static void register_sighandler(int sig) {
    struct sigaction act = {0};
    tracef("Registering handler for signal %d.", sig);

    act.sa_handler = handle_sig;
    sigaction(sig, &act, NULL);
}

static int repl() {

    int retval;
    do {
        retval = yyparse();
        tracef("yyparse() -> %d", retval);
    } while (retval != 0);

    return last_exit_code();
}

static int shell(int input_fd, bool interactive) {
    tracef("Starting parser (fd=%d, interactive=%d)", input_fd, interactive);
    shell_input_fd_ = input_fd;
    shell_interactive_ = interactive;

    if (interactive) {
        return repl();
    }

    int parse_err = yyparse();
    return parse_err ? parse_err : last_exit_code();
}

void lexer_input(char *buffer, int *n_bytes, int max_bytes) {
    static bool eof = false;
    int read_ret;
    *n_bytes = 0;

    if (eof) {
        tracef("%s", "read() -> EOF");
        return;
    }

    if (!shell_interactive_) {
        if (input_string_) {
            read_ret = strlen(input_string_);
            read_ret = read_ret > max_bytes ? max_bytes : read_ret;
            memcpy(buffer, input_string_, read_ret);
            input_string_ += read_ret;
            *n_bytes = read_ret;
        } else {
            read_ret = read(shell_input_fd_, buffer, max_bytes);
            *n_bytes = (read_ret >= 0) * read_ret;
        }

    } else {
    prompt:
        fprintf(stderr, "mysh$ ");
        fflush(stderr);

        char c;
        do {
            read_ret = read(shell_input_fd_, &c, 1);
            tracef("read(%c) -> %d", c, read_ret);

            switch (read_ret) {
            case 1: {
                buffer[*n_bytes] = c;
                *n_bytes += 1;
            } break;
            case 0: // end of input
                eof = true;
                fprintf(stderr, "\n");
                return;
            default: { // read interrupted
                tracef("errno = %d", errno);
                if (errno == EINTR) {
                    fprintf(stderr, "\n");
                    goto prompt;
                }
            } break;
            }
        } while (c != '\n' && *n_bytes != max_bytes);
    }
}
