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
    double num;
    char *name;
    alist_t names;
}

%token                  SEMIC               "';'"
%token                  ENDL                "<newline>"
%token                  YYEOF               "<end-of-file>"
%token <name>           NAME                "identifier"
%token <name>           VAR                 "variable"

%type <names>           cmd
%type <names>           value_list
%type <name>            value

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
cmd
    : value_list
;
value_list
    : value             { $$ = alist_create(6, sizeof(char*));
                            alist_append($$, char*, $1); }
    | value_list value  { alist_append($1, char*, $2); $$ = $1; }
;
value
    : NAME
    | VAR               { $$ = sem_expand_var($1); }
;

dumper
    : dumper_token
    | dumper dumper_token
;
dumper_token
    : YYEOF { printf("%d:%d: YYEOF\n", @1.first_line, @1.first_column); }
    | NAME { printf("%d:%d: NAME: %s\n", @1.first_line, @1.first_column, $1); }
    | VAR { printf("%d:%d: VAR: %s\n", @1.first_line, @1.first_column, $1); }
    | ENDL { printf("%d:%d: ENDL\n", @1.first_line, @1.first_column); }
    | SEMIC { printf("%d:%d: SEMIC\n", @1.first_line, @1.first_column); }
;
%%
// Epilogue
#include "parser-epilogue.inc"
