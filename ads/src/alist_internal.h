#ifndef _ALIST_INTERNAL_H
#define _ALIST_INTERNAL_H

/**
 * An inflatable array list.
 */
struct alist {
    alist_size_t count;
    alist_size_t capacity;
    alist_size_t itemSize;
    void* contents;
};

#endif