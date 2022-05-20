#ifndef _PARSER_EPILOGUE_INC
#define _PARSER_EPILOGUE_INC

#ifndef YYBISON
#include "../src/parser.yy.c"
#endif

#include <err.h>

int yyerror(YYLTYPE *loc, const char *message) {
    warnx("Syntax error:%d.%d: %s\n", loc->first_line, loc->first_column,
          message);

    return 0;
}

static int yyreport_syntax_error(const yypcontext_t *ctx) {
    extern const char *program_argv_0;
    int res = 0;
    fprintf(stderr, "%s:", program_argv_0);
    YYLOCATION_PRINT(stderr, yypcontext_location(ctx));
    fprintf(stderr, ": syntax error");
    // Report the tokens expected at this point.
    {
        enum { TOKENMAX = 5 };
        yysymbol_kind_t expected[TOKENMAX];
        int n = yypcontext_expected_tokens(ctx, expected, TOKENMAX);
        if (n < 0) {
            // Forward errors to yyparse.
            res = n;
        } else {
            for (int i = 0; i < n; ++i) {
                fprintf(stderr, "%s %s", i == 0 ? ": expected" : " or",
                        yysymbol_name(expected[i]));
            }
        }
    }
    // Report the unexpected token.
    {
        yysymbol_kind_t lookahead = yypcontext_token(ctx);
        if (lookahead != YYSYMBOL_YYEMPTY)
            fprintf(stderr, " before %s", yysymbol_name(lookahead));
    }
    fprintf(stderr, "\n");
    return res;
}

#endif