#include "command.h"
#include "debug.h"
#include "utils.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <llist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int exit_code_ = 0;
int last_exit_code() { return exit_code_; }

/**
 * @brief Runs command with specified arguments.
 *
 * @param argv Null-terminated list of arguments.
 * @param exit_code Output param with exit code of the command.
 * @returns Status of the command execution.
 */
runerr_t run_command(struct command_info *cmd, int *exit_code) {

    int argc = alist_count(cmd->argv);
    char **argv = &alist_ref(cmd->argv, char *, 0);

    assert(argc >= 1);
    tracef("(%s, argc=%d)", argv[0], argc);

    runerr_t retval = run_internal_command(argv, argc, exit_code);

    if (retval == RUNERR_NOCMD) {
        // try external command
        pid_t pid = fork();

        if (cmd->pipe_out) {
            pipe(cmd->pipefds);
        }

        if (pid == -1) {
            err(1, "fork");
        } else if (pid == 0) {
            // child

            // pipe redirect
            if (cmd->pipe_in) {
                int fd = cmd->pipe_in->pipefds[0];
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (cmd->pipe_out) {
                int fd = cmd->pipefds[1];
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // file redirect
            if (cmd->redir_in.type != REDIR_TYPE_NONE) {
                int fd = open(cmd->redir_in.filename, O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (cmd->redir_out.type != REDIR_TYPE_NONE) {
                int flags =
                    O_CREAT | O_WRONLY |
                    (cmd->redir_out.type == REDIR_TYPE_APPEND ? O_APPEND : 0);
                int fd = open(cmd->redir_out.filename, flags, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // execvp needs nul-terminated
            alist_append(cmd->argv, char *, NULL);
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

        retval = RUNERR_OK;
    }

    tracef("exit_code=%d", *exit_code);
    exit_code_ = *exit_code;
    return retval;
}

runerr_t run_internal_command(char **argv, int argc, int *exit_code) {
    assert(argc >= 1);
#define if_command(cmd) if (strcmp(argv[0], cmd) == 0)

    if_command("exit") {
        u_int8_t code = last_exit_code();
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

struct command_info *command_info_create() {
    struct command_info *info = malloc_chk(sizeof(struct command_info));
    *info = (struct command_info){0};
    info->argv = alist_create(6, sizeof(char *));
    return info;
}
void command_info_destroy(struct command_info *info) {

    while (info) {
        alist_foreach(info->argv, char *, it) { free(*it); }
        alist_destroy(info->argv);
        free(info->redir_in.filename);
        free(info->redir_out.filename);
        struct command_info *next = info->pipe_out;
        free(info);

        info = next;
    }
}