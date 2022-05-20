#include "parser.h"
#include "command.h"
#include "parser.yy.h"
#include "utils.h"

#include <alist.h>

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void sem_run_command(struct command_info *cmd) {

    while (cmd->piped_from) cmd = cmd->piped_from;

    int excode;
    runerr_t runerr = run_command(cmd, &excode);

    command_info_destroy(cmd);

    if (runerr == RUNERR_REQUEST_EXIT) {
        exit(excode);
    }
}

struct command_info *sem_cmd_from_parts(struct command_info *info,
                                        struct command_part *part) {
    assert(part);
    if (info == NULL) {
        info = command_info_create();
    }

    switch (part->type) {
    case CMDPART_VALUE:
        alist_append(info->argv, char *, part->value);
        break;
    case CMDPART_REDIRECT:
        switch (part->redir.type) {
        case REDIR_TYPE_OUT:
        case REDIR_TYPE_APPEND:
            info->redir_out = part->redir;
            break;
        case REDIR_TYPE_IN:
            info->redir_in = part->redir;
            break;
        default:
            break;
        }
        break;
    }

    free(part);
    return info;
}

struct command_info *sem_cmd_pipe_to(struct command_info *info,
                                     struct command_part *part) {
    assert(info && part);
    info->piped_to = sem_cmd_from_parts(NULL, part);
    info->piped_to->piped_from = info;

    return info->piped_to;
}

struct command_part *sem_name(char *name) {
    struct command_part *part = malloc_chk(sizeof(struct command_part));
    part->type = CMDPART_VALUE;
    part->value = name;
    return part;
}

struct command_part *sem_redir_in(char *filename) {
    struct command_part *part = malloc_chk(sizeof(struct command_part));
    part->type = CMDPART_REDIRECT;
    part->redir.type = REDIR_TYPE_IN;
    part->redir.filename = filename;
    return part;
}

struct command_part *sem_redir_out(char *filename) {
    struct command_part *part = malloc_chk(sizeof(struct command_part));
    part->type = CMDPART_REDIRECT;
    part->redir.type = REDIR_TYPE_OUT;
    part->redir.filename = filename;
    return part;
}

struct command_part *sem_redir_outa(char *filename) {
    struct command_part *part = malloc_chk(sizeof(struct command_part));
    part->type = CMDPART_REDIRECT;
    part->redir.type = REDIR_TYPE_APPEND;
    part->redir.filename = filename;
    return part;
}

struct command_part *sem_expand_var(char *varname) {
    char buf[16];
    if (strcmp(varname, "?") == 0) {
        sprintf(buf, "%d", last_exit_code());
        free(varname);
        return sem_name(sem_strdup(buf));
    }
    if (strcmp(varname, "$") == 0) {
        sprintf(buf, "%d", getpid());
        free(varname);
        return sem_name(sem_strdup(buf));
    }
    char *result = getenv(varname);
    free(varname);
    if (result) {
        return sem_name(sem_strdup(result));
    }
    return sem_name(sem_strdup(""));
}

char *sem_strdup(char *str) {
    size_t len = strlen(str);
    char *result = malloc_chk(sizeof(char) * (len + 1));
    for (size_t i = 0; i <= len; i++) {
        result[i] = str[i];
    }
    return result;
}

void command_part_destroy(struct command_part *part) {
    switch (part->type) {
    case CMDPART_VALUE:
        free(part->value);
        break;
    case CMDPART_REDIRECT:
        free(part->redir.filename);
        break;
    }
    free(part);
}
void free_redir_info(struct redir_info *info) {
    free(info->filename);
    free(info);
}