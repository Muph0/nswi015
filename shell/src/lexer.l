%{
// User declarations
#include "lexer.h"

#include <stdio.h>

#define YY_USER_ACTION { \
    loc_next(yylloc, yytext); \
}

%}

%option noyywrap
%option nounput
%option noinput

%x LINE_COMMENT

WHITESPACE [ \t]+
NAME_CHAR [^ \n\t;|\{\}]

%%

<LINE_COMMENT>{
"\n"            { BEGIN(INITIAL); return ENDL; }
.               { }
}

";"             { return SEMIC; }
"\n"            { return ENDL; }
"#"             { BEGIN(LINE_COMMENT); }

{NAME_CHAR}+    { yylval->name = strdup(yytext); return NAME; }
${NAME_CHAR}+   { yylval->name = strdup(yytext + 1); return VAR; }

{WHITESPACE}    { }
.               { LEXERR("Unknown symbol '%s'.\n", yytext); return ENDL; }

<<EOF>>         { return YYEOF; }

