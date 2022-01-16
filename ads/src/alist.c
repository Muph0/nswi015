#include "alist.h"
#include "alist_internal.h"

#include <stdlib.h>


alist_size_t alist_count(alist_t list) {
    return list->count;
}

bool alist_resize(alist_t list, alist_size_t newCount) {
    if (!alist_ensure_capacity(list, newCount)) {
        return false;
    }

    list->count = newCount;
    return true;
}

bool alist_ensure_capacity(alist_t list, alist_size_t capacity) {
    if (list->capacity < capacity) {
        alist_size_t newCap = list->capacity * 2;
        newCap = newCap > capacity ? newCap : capacity;
        void* newContents = malloc(list->itemSize * newCap);
        if (newContents == NULL) {
            return false;
        }

        if (list->contents != NULL) {
            memcpy(newContents, list->contents, list->capacity * list->itemSize);
            free(list->contents);
        }

        list->contents = newContents;
        list->capacity = newCap;
    }

    return true;
}

alist_t alist_create(alist_size_t initialCapacity, alist_size_t itemSize) {
    assert(initialCapacity > 0);
    alist_t list = (alist_t)malloc(sizeof(struct alist));

    if (list != NULL) {
        list->count = 0;
        list->itemSize = itemSize;
        list->capacity = 0;
        list->contents = NULL;
        alist_ensure_capacity(list, initialCapacity);
    }

    return list;
}

void alist_destroy(alist_t list) {
    if (list->contents)
        free(list->contents);
    free(list);
}

void alist_clear(alist_t list) {
    list->count = 0;
}

void* alist_ref_safe(alist_t list, alist_size_t index, alist_size_t itemSize) {
    assert(list->itemSize == itemSize);
    assert(index < list->count);

    return list->contents + index * itemSize;
}

void* alist_begin_safe(alist_t list, alist_size_t itemSize) {
    assert(list->itemSize == itemSize);
    return list->contents;
}
void* alist_end_safe(alist_t list, alist_size_t itemSize) {
    assert(list->itemSize == itemSize);
    return list->contents + list->count * itemSize;
}

void alist_append_safe(alist_t list, alist_size_t itemSize, void* item) {
    assert(list->itemSize == itemSize);

    bool success = alist_ensure_capacity(list, list->count + 1);
    assert(success);

    memcpy(list->contents + list->count * itemSize, item, itemSize);
    list->count++;
}

void* alist_insert_safe(alist_t list, alist_size_t index, alist_size_t itemSize) {
    assert(list->itemSize == itemSize);
    assert(index <= list->count);

    void* source = list->contents + index * itemSize;
    void* dest = source + itemSize;
    size_t length = itemSize * (list->count - index);

    alist_ensure_capacity(list, index + 1);
    if (length > 0)
        memmove(dest, source, length);

    list->count++;
    return source;
}

void alist_remove_safe(alist_t list, alist_size_t index, alist_size_t itemSize) {
    assert(list->itemSize == itemSize);
    assert(index < list->count);
    assert(list->count > 0);

    void* dest = list->contents + index * itemSize;
    void* source = dest + itemSize;
    size_t length = itemSize * (list->count - index - 1);

    if (length > 0)
        memmove(dest, source, length);

    list->count--;
}
