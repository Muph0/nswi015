%{
// User declarations
#include "lexer.h"

#include <stdio.h>

#define YY_USER_ACTION { \
    loc_next(yylloc, yytext); \
}


/*"\>\>{s}*{VALUE}" { yylval->part = sem_redir_outa(sem_strdup(yytext)); return REDIR_OUTA; }*/
/*"\>{s}*{VALUE}"	{ yylval->part = sem_redir_out(sem_strdup(yytext)); return REDIR_OUT; }*/
/*"\<{s}*{VALUE}"	{ yylval->part = sem_redir_in(sem_strdup(yytext)); return REDIR_IN; }*/

%}

%option noyywrap
%option nounput
%option noinput

%x LINE_COMMENT

s               [ \t]+
VALUE_CHAR      [^ \n\t;\|\$\{\}]
VALUE           [^\<\> \n\t;\|\$\{\}]{VALUE_CHAR}*

%%

<LINE_COMMENT>{
"\n"			{ BEGIN(INITIAL); return ENDL; }
.				{ }
}

";"				{ return SEMIC; }
"|"				{ return PIPE; }
"\n"			{ return ENDL; }
"\<"            { return REDIR_IN; }
"\>"            { return REDIR_OUT; }
"\>\>"          { return REDIR_OUTA; }
"#"				{ BEGIN(LINE_COMMENT); }

{VALUE}	        { yylval->value = sem_strdup(yytext); return VALUE; }
\${VALUE}	    { yylval->value = sem_strdup(yytext + 1); return VAR; }
\$\$			{ yylval->value = sem_strdup("$"); return VAR; }

{s}	            { }
.				{ LEXERR("Unknown symbol '%s'.\n", yytext); return ENDL; }

<<EOF>>			{ return YYEOF; }

