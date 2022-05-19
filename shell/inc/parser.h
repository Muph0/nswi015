#ifndef _PARSER_H
#define _PARSER_H

#include <alist.h>

struct YYLTYPE;

// flex/bison interaction
int yyparse(void);
int yyerror(struct YYLTYPE *, const char*);
void lexer_input(char *, int *, int);

// semantic actions
void sem_run_command(alist_t);
char* sem_strdup(char*);
char* sem_expand_var(char*);

#endif
