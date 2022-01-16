#include "command.h"
#include "debug.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int exit_code_;
int last_exit_code() { return exit_code_; }

/**
 * @brief Runs command with specified arguments.
 *
 * @param argv Null-terminated list of arguments.
 * @return Returns exit code of the command.
 */
runerr_t run_command(char **argv, int argc, int *exit_code) {
    assert(argc >= 1);
    tracef("(%s, argc=%d)", argv[0], argc);

    runerr_t retval = run_internal_command(argv, argc, exit_code);

    if (retval == RUNERR_NOCMD) {
        // try external command
        int pid = fork();

        if (pid == -1) err(1, "fork");
        else if (pid == 0) {
            // child
            execvp(argv[0], argv);
            // if returns, print error
            err(127, "unknown command");
        }

        // parent
        tracef("fork() -> %d", pid);

        int status, cpid;
        do {
            tracef("wait(%d)", pid);
            cpid = wait(&status);
            tracef("wait returned (child=%d, status=%u)", cpid, status);

            if (WIFEXITED(status)) {
                *exit_code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                // |128 according to bash
                *exit_code = WTERMSIG(status) | 128;
            }
            // child gets killed magically on its own (?)
        } while (cpid != pid);
    }

    tracef("exit_code=%d", *exit_code);
    exit_code_ = *exit_code;
    return RUNERR_OK;
}

runerr_t run_internal_command(char **argv, int argc, int *exit_code) {
#define if_command(cmd) if (strcmp(argv[0], cmd) == 0)
    assert(argc >= 1);

    if_command("exit") {
        u_int8_t code = 0;
        if (argc > 1) {
            char *end;
            code = strtol(argv[1], &end, 10);

            if (*end != '\0') {
                warnx("exit: Illegal number: %s", argv[1]);
                *exit_code = 2;
                return RUNERR_OK;
            }
        }
        *exit_code = code;
        return RUNERR_REQUEST_EXIT;
    }
    if_command("cd") {
        *exit_code = 0;

        char *newpwd = (argc == 2) //
                           ? argv[1]
                           : getenv("HOME");

        switch (argc) {
        default:
            warnx("cd: too many arguments");
            *exit_code = 1;
            break;

        case 1:
        case 2: {
            if (!newpwd) {
                warnx("cd: HOME not set");
                *exit_code = 1;
                break;
            }

            // expand "-" to $OLDPWD
            if (strcmp(newpwd, "-") == 0) {
                newpwd = getenv("OLDPWD");
                if (!newpwd) {
                    warnx("cd: OLDPWD not set");
                    *exit_code = 1;
                    break;
                }
                printf("%s\n", newpwd);
            }

            // perform the chdir
            char *oldpwd = get_current_dir_name();
            tracef("cd: %s -> %s", oldpwd, newpwd);
            if (chdir(newpwd)) {
                *exit_code = 1;
                warn("cd: %s", newpwd);
                break;
            }
            newpwd = get_current_dir_name();
            setenv("OLDPWD", oldpwd, true);
            setenv("PWD", newpwd, true);
            free(newpwd);
            free(oldpwd);
        } break;
        }
        return RUNERR_OK;
    }

    return RUNERR_NOCMD;
#undef if_command
}