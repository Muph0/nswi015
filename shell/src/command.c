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

static char *get_current_dir_name();

static int exit_code_ = 0;
int last_exit_code() { return exit_code_; }


#define trace_exec(argv, argc) \
    switch (argc) { \
    case 1: \
        tracef("child exec: %s", argv[0]); \
        break; \
    case 2: \
        tracef("child exec: %s %s", argv[0], argv[1]); \
        break; \
    case 3: \
        tracef("child exec: %s %s %s", argv[0], argv[1], argv[2]); \
        break; \
    default: \
        tracef("child exec: %s %s %s... (%d more)", argv[0], argv[1], argv[2], \
               argc - 3); \
        break; \
    }


/**
 * @brief Runs command with specified arguments.
 *
 * @param argv Null-terminated list of arguments.
 * @param exit_code Output param with exit code of the command.
 * @returns Status of the command execution.
 */
runerr_t run_command(struct command_info *cmdinfo, int *exit_code) {

    assert(cmdinfo);
    runerr_t retval =
        run_internal_command(&alist_ref(cmdinfo->argv, char *, 0),
                             alist_count(cmdinfo->argv), exit_code);

    if (retval == RUNERR_NOCMD) {
        // try external command

        pid_t pid;
        int pd_0 = -1;
        int children = 0;

        for (struct command_info *cmd = cmdinfo; //
             cmd != NULL; cmd = cmd->piped_to) {

            alist_append(cmd->argv, char *, (char *)NULL);
            char **argv = alist_begin(cmd->argv, char *);
            int argc = alist_count(cmd->argv) - 1;

            int first = cmd->piped_from == NULL;
            int last = cmd->piped_to == NULL;

            int pd[2];
            if (!last) {
                if (pipe(pd) == -1) err(1, "pipe");
                tracef("pipe([r:%d, w:%d])", pd[0], pd[1]);
            }

            switch (pid = fork()) {
            case -1:
                err(1, "fork");
            case 0: // child
                // pipe redirect
                if (!last) {
                    dup2(pd[1], 1);
                    tracef("child: dup2(%d, %d)", pd[1], 1);
                    close(pd[0]);
                    close(pd[1]);
                    tracef("child: close(%d, %d)", pd[0], pd[1]);
                }
                if (!first) {
                    dup2(pd_0, 0);
                    close(pd_0);
                    tracef("child: dup2(%d, %d)", pd_0, 0);
                    tracef("child: close(%d)", pd_0);
                }

                // file redirect
                if (cmd->redir_in.type != REDIR_TYPE_NONE) {
                    int fd = open(cmd->redir_in.filename, O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    printf("child: &%d < %s", STDIN_FILENO,
                           cmd->redir_in.filename);
                }
                if (cmd->redir_out.type != REDIR_TYPE_NONE) {
                    int append = cmd->redir_out.type == REDIR_TYPE_APPEND;
                    int flags = O_CREAT | O_WRONLY | (append ? O_APPEND : 0);
                    int fd = open(cmd->redir_out.filename, flags, 0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    printf("child: &%d %s %s", STDIN_FILENO,
                           append ? ">>" : ">", cmd->redir_in.filename);
                }

                trace_exec(argv, argc);
                execvp(argv[0], argv);
                err(127, "unknown command");
            default:
                children++;
                if (!first) {
                    close(pd_0);
                    tracef("parent: close(%d)", pd_0);
                }
                if (!last) {
                    close(pd[1]);
                    tracef("parent: close(%d)", pd[1]);
                    pd_0 = pd[0];
                    tracef("parent: passing pipefd %d to next child", pd_0);
                }
                break;
            }
        }

        // parent
        int waitfor = children;
        while (waitfor) {
            int status, cpid;
            tracef("parent: waiting for %d children", waitfor);
            cpid = wait(&status);
            waitfor--;
            tracef("parent: wait returned (child=%d, status=%u)", cpid, status);

            if (cpid == pid) { // pid of the last child
                if (WIFEXITED(status)) {
                    *exit_code = WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    // | 128 according to bash
                    *exit_code = WTERMSIG(status) | 128;
                }
            }
        }
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
        struct command_info *next = info->piped_to;
        free(info);

        info = next;
    }
}

static char *get_current_dir_name() {
    size_t size = 64;
    char *buf = malloc_chk(size);
    char *result = NULL;
    while (result == NULL) {
        result = getcwd(buf, size);
        if (result == NULL && errno != ERANGE) {
            err(1, "couldn't get working directory name");
        }
    }
    return result;
}
