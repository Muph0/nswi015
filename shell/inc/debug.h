#ifndef _DEBUG_H
#define _DEBUG_H

#include <string.h>

//#define TRACE

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x)

#ifndef TRACE
#define tracef(...)
#else
#define tracef(...) \
    do { \
        if (!_tracef_hidden(__func__)) { \
            fprintf(stderr, "\x1b[1;30m%s:%d: ", __func__, __LINE__); \
            fprintf(stderr, __VA_ARGS__); \
            fprintf(stderr, "\x1b[0m\n"); \
        } \
    } while (0)

static const char *_tracef_hidden_traces[] = {
    "yyparse",
    "lexer_input",
    "repl",
    "handle_sig+",
    "run_command+",
};

static int _tracef_hidden(const char *name) {
    //if (!TRACE_ENABLED_) return 1;
    for (unsigned int i = 0;
         i < sizeof(_tracef_hidden_traces) / sizeof(*_tracef_hidden_traces);
         i++) {
        if (strcmp(_tracef_hidden_traces[i], name) == 0) return 1;
    }
    return 0;
}
#endif

#endif