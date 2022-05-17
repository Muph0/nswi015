#ifndef _ALIST_H
#define _ALIST_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define ALIST_DEFAULT_CAP ((alist_size_t)16)


#define alist_type_info(T) sizeof(T)
typedef ssize_t alist_size_t;
typedef alist_size_t alist_type_info_t;

/**
 * An inflatable array list.
 */
typedef struct alist* alist_t;

bool alist_ensure_capacity(alist_t list, alist_size_t capacity);
bool alist_resize(alist_t list, alist_size_t newCount);

/**
 * @brief Get the size of a list.
 *
 * @param list The list.
 * @return Count of items in the list.
 */
alist_size_t alist_count(alist_t list);

alist_t alist_create(alist_size_t initialCapacity, alist_type_info_t itemSize);
void alist_destroy(alist_t list);

void alist_clear(alist_t list);

/**
 * @brief Get reference to an item in list.
 *
 * @param list The list to read from.
 * @param index Index of the item.
 * @param itemSize Size of the item type.
 * @return Pointer to the item in list.
 */
void* alist_ref_safe(alist_t list, alist_size_t index, alist_type_info_t itemSize);
#define alist_ref(list, T, index) (*((T*)alist_ref_safe(list, index, alist_type_info(T))))

/**
 * @brief Append an item to the end of a list.
 *
 * @param list The list to append to
 * @param itemSize Size of the item type.
 * @param item The item.
 * @return The new size
 */
void alist_append_safe(alist_t list, alist_type_info_t itemSize, void* item);
#define alist_append(list, T, item) do { \
        T _copy = item; \
        alist_append_safe (list, alist_type_info(T), &_copy); \
    } while (0);

/**
 * @brief Insert an item into the list.
 *
 * @param list The list to insert into.
 * @param index The index of the inserted item.
 * @param itemSize Item size.
 * @return Pointer to the newly inserted item.
 */
void* alist_insert_safe(alist_t list, alist_size_t index, alist_type_info_t itemSize);
#define alist_insert(list, T, index) (*(T*)alist_insert_safe(list, index, alist_type_info(T)))

void alist_remove_safe(alist_t list, alist_size_t index, alist_type_info_t itemSize);
#define alist_remove(list, T, index) alist_remove_safe(list, index, alist_type_info(T))

#define alist_push_back(list, T, item) (alist_insert(list, T, alist_count(list)) = (item))
#define alist_push_front(list, T, item) (alist_insert(list, T, 0) = (item))
#define alist_pop_back(list, T) (alist_remove(list, T, alist_count(list)-1))
#define alist_pop_front(list, T) (alist_remove(list, T, 0))

void* alist_begin_safe(alist_t list, alist_type_info_t itemSize);
void* alist_end_safe(alist_t list, alist_type_info_t itemSize);

#define alist_begin(list, T) ((T*)alist_begin_safe(list, alist_type_info(T)))
#define alist_end(list, T) ((T*)alist_end_safe(list, alist_type_info(T)))

#define alist_foreach(list, T, it) \
    for (T* it = alist_begin(list, T); it < alist_end(list, T); it++)


#endif