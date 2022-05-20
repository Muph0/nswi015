#ifndef _DEBUG_H
#define _DEBUG_H

#include <string.h>

//#define TRACE

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x)

#ifndef TRACE
#define tracef(...)
#else
#define tracef(format, ...) \
    do { \
        if (!_tracef_hidden(__func__)) { \
            fprintf(stderr, "\x1b[1;30m%s:%d: " format "\x1b[0m\n", \
             __func__, __LINE__, __VA_ARGS__); \
        } \
    } while (0)

static const char *_tracef_hidden_traces[] = {
    "yyparse",
    "lexer_input",
    "repl",
    "run_command+",
};

static int _tracef_hidden(const char *name) {
    for (unsigned int i = 0;
         i < sizeof(_tracef_hidden_traces) / sizeof(*_tracef_hidden_traces);
         i++) {
        if (strcmp(_tracef_hidden_traces[i], name) == 0) return 1;
    }
    return 0;
}
#endif

#endif