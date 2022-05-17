#ifndef _RUNNER_H
#define _RUNNER_H

#include <stdbool.h>

enum runerr {
    RUNERR_OK = 1, // Command completed successfully.
    RUNERR_NOCMD, // Command was not found.
    RUNERR_REQUEST_EXIT, // Command completed and requested shell to exit.
};

typedef enum runerr runerr_t;

int last_exit_code();
runerr_t run_command(char **, int, int*);
runerr_t run_internal_command(char **, int, int*);

#endif
