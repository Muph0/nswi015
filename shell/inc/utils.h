#ifndef _UTILS_H
#define _UTILS_H

#include <unistd.h>
#include <stdlib.h>
#include <err.h>

static void *malloc_chk(size_t size) {
    void *result = malloc(size);
    if (result == NULL) err(1, "out of memory");
    return result;
}

#endif