#ifndef _RUNNER_H
#define _RUNNER_H

#include <stdbool.h>

enum runerr {
    RUNERR_OK = 1,
    RUNERR_NOCMD,
    RUNERR_REQUEST_EXIT,
};

typedef enum runerr runerr_t;

int last_exit_code();
runerr_t run_command(char **, int, int*);
runerr_t run_internal_command(char **, int, int*);

#endif
