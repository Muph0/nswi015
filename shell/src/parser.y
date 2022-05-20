%code requires {
// Header
#include <alist.h>
#define YY_DECL int yylex(YYSTYPE* yylval, struct YYLTYPE* yylloc)
}

%{
// Prologue
#include "debug.h"
#include "parser.h"
#include "parser.yy.h"

#include <alist.h>
#include <math.h>
#include <stdio.h>

YY_DECL;
%}

%define api.pure full
%locations
%define parse.error custom

%union {
    char *value;
    alist_t values;
    struct command_part *part;
    struct command_info *command;
    int number;
}

%token                  SEMIC               "';'"
%token                  PIPE                "'|'"
%token                  REDIR_IN            "'<'"
%token                  REDIR_OUT           "'>'"
%token                  REDIR_OUTA          "'>>'"
%token                  ENDL                "<newline>"
%token                  YYEOF               "<end-of-file>"
%token <value>          VALUE               "identifier"
%token <value>          VAR                 "variable"

%type <command>         cmd
%type <part>            cmdpart
%type <part>            pipe_seq

%start unit

%%
// Grammar rules

unit
    : lines YYEOF       { tracef("EOF"); }
;
lines
    : line
    | lines ENDL line
;
line
    : %empty
    | cmd_seq
    | cmd_seq SEMIC
;
cmd_seq
    : cmd                   { sem_run_command($1); }
    | cmd_seq SEMIC cmd     { sem_run_command($3); }
;
//pipe_seq
//    : cmd                   { $$ = sem_run_command_to_pipe($1, -1); }
//    | pipe_seq cmd PIPE     { $$ = sem_run_command_to_pipe($2, $1); }
//;
cmd
    : cmdpart           { $$ = sem_cmd_from_parts(NULL, $1); }
    | cmd cmdpart       { $$ = sem_cmd_from_parts($1, $2); }
    | cmd PIPE cmdpart  { $$ = sem_cmd_pipe_to($1, $2); }
;
cmdpart
    : VALUE             { $$ = sem_name($1); }
    | VAR               { $$ = sem_expand_var($1); }
    | REDIR_IN VALUE    { $$ = sem_redir_in($2); }
    | REDIR_OUT VALUE   { $$ = sem_redir_out($2); }
    | REDIR_OUTA VALUE  { $$ = sem_redir_outa($2); }
;

dumper
    : dumper_token
    | dumper dumper_token
;
dumper_token
    : YYEOF { printf("%d:%d: YYEOF\n", @1.first_line, @1.first_column); }
    | VALUE { printf("%d:%d: VALUE: %s\n", @1.first_line, @1.first_column, $1); }
    | VAR { printf("%d:%d: VAR: %s\n", @1.first_line, @1.first_column, $1); }
    | ENDL { printf("%d:%d: ENDL\n", @1.first_line, @1.first_column); }
    | SEMIC { printf("%d:%d: SEMIC\n", @1.first_line, @1.first_column); }
;
%%
// Epilogue
#include "parser-epilogue.inc"
