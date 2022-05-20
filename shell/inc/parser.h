#ifndef _PARSER_H
#define _PARSER_H

#include <alist.h>

#include "command.h"
#include "utils.h"

struct YYLTYPE;

// flex/bison interaction
int yyparse(void);
int yyerror(struct YYLTYPE *, const char *);
void lexer_input(char *, int *, int);

enum command_part_type {
    CMDPART_VALUE,
    CMDPART_REDIRECT,
};

struct command_part {
    enum command_part_type type;
    union {
        char *value;
        struct redir_info redir;
    };
};

static struct command_part *command_part_create(enum command_part_type type) {
    struct command_part *result = malloc_chk(sizeof(struct command_part));
    result->type = type;
    return result;
}
void command_part_destroy(struct command_part *);

// semantic actions
void sem_run_command(struct command_info *);

struct command_info *sem_cmd_from_parts(struct command_info *,
                                    struct command_part *);
struct command_info *sem_cmd_pipe_to(struct command_info *,
                                    struct command_part *);

struct command_part *sem_name(char *);
struct command_part *sem_expand_var(char *);
struct command_part *sem_redir_in(char *);
struct command_part *sem_redir_out(char *);
struct command_part *sem_redir_outa(char *);


char *sem_strdup(char *);

void free_redir_info(struct redir_info *);

#endif
