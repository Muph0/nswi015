#include "parser.h"
#include "parser.yy.h"
#include "command.h"

#include <alist.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void sem_run_command(alist_t list) {
    alist_append(list, char *, NULL);

    char **argv = &alist_ref(list, char *, 0);
    int excode;
    runerr_t runerr = run_command(argv, alist_count(list) - 1, &excode);

    alist_foreach(list, char *, str) {
        // created in lexer
        if (*str) free(*str);
    }

    // created in parser
    alist_destroy(list);

    if (runerr == RUNERR_REQUEST_EXIT) {
        exit(excode);
    }
}

char *sem_expand_var(char *name) {
    char buf[16];
    if (strcmp(name, "?") == 0) {
        sprintf(buf, "%d", last_exit_code());
        return strdup(buf);
    }
    if (strcmp(name, "$") == 0) {
        sprintf(buf, "%d", getpid());
        return strdup(buf);
    }

    char *result = getenv(name);
    if (result) {
        return strdup(result);
    }
    return strdup("");
}