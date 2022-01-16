#ifndef _LEXER_H
#define _LEXER_H

#include "parser.h"
#include "parser.yy.h"

#define LEXERR(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#define YY_INPUT(b, r, s) lexer_input(b, &r, s)

static void loc_next(YYLTYPE *yylloc, char *yytext) {
    static YYLTYPE curloc = {
        .first_line = 1,
        .first_column = 0,
        .last_line = 1,
        .last_column = 0,
    };

    for (char *it = yytext; *it; it++) {
        switch (*it) {
        case '\n':
            curloc.last_column = 0;
            curloc.last_line++;
            break;
        case '\t':
            curloc.last_column += 4;
            break;
        default:
            curloc.last_column++;
            break;
        }
    }

    *yylloc = curloc;
    curloc.first_line = curloc.last_line;
    curloc.first_column = curloc.last_column;
}

#endif
