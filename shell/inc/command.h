#ifndef _RUNNER_H
#define _RUNNER_H

#include <alist.h>
#include <stdbool.h>

enum runerr {
    RUNERR_OK = 1,       // Command completed successfully.
    RUNERR_NOCMD,        // Command was not found.
    RUNERR_REQUEST_EXIT, // Command completed and requested shell to exit.
};

typedef enum runerr runerr_t;

enum redir_type {
    REDIR_TYPE_NONE = 0,
    REDIR_TYPE_IN,
    REDIR_TYPE_OUT,
    REDIR_TYPE_APPEND,
};

struct redir_info {
    char *filename;
    enum redir_type type;
};

struct command_info {
    alist_t argv;
    struct redir_info redir_out;
    struct redir_info redir_in;

    struct command_info *piped_to; // Next command after PIPE token.
    struct command_info *piped_from; // Previous command before PIPE token.
};

int last_exit_code();
runerr_t run_command(struct command_info *, int *);
runerr_t run_internal_command(char **, int, int *);

struct command_info *command_info_create();
void command_info_destroy(struct command_info *);

#endif
