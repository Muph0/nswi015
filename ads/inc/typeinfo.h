#ifndef _TYPEINFO_H
#define _TYPEINFO_H

#include <stddef.h>
#include <string.h>

#define ___nameof(x) #x
#define nameof(x) ___nameof(x)

#define typeof(x) typeinfo_t { .name = nameof(x), .size = sizeof(x) };

typedef struct {

    /**
     * @brief Name of the type.
     */
    const char* const name;

    /**
     * @brief Size of the type.
     */
    const size_t size;

} typeinfo_t;

static bool typeinfo_cmp(const typeinfo_t* left, const typeinfo_t* right) {
    int cmp = -(left->size < right->size) | (left->size > right->size);

    if (!cmp) {
        cmp = strcmp(left->name, right->name);
    }

    return cmp;
}

#endif
