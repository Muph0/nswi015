#ifndef _LLIST_H
#define _LLIST_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "struct.h"

/**
 * Polymorphic list link
 */
typedef struct link {
    struct link* prev;
    struct link* next;
} llink_t;

/**
 * Inlined doubly linked list data structure.
 */
typedef struct {
    llink_t head;
} llist_t;

/**
 * Get a typed structure from the given list item.
 *
 * @param link   Pointer to the list item.
 * @param type   The type of the structure the item is embedded in.
 * @param member The name of the member of the item within
 *               the containing structure.
 *
 * @return Typed structure corresponding to the list item.
 */
#define llist_item(LINK, TYPE, MEMBER) \
    struct_container_of(LINK, TYPE, MEMBER)

/**
 * Iterate over list members
 *
 * @param list     The list to iterate over.
 * @param type     The type of the structure the item is embedded in.
 * @param member   The name of the member of the item within
 *                 the containing structure.
 * @param iterator The name of the iterator to declare.
 */
#define llist_foreach(LIST, TYPE, MEMBER, ITERATOR) \
    for (TYPE* ITERATOR = NULL; ITERATOR == NULL; ITERATOR = (TYPE*)1) \
        for (llink_t* _link = (LIST).head.next; \
                ITERATOR = llist_item(_link, TYPE, MEMBER), \
                     _link != &(LIST).head; \
                _link = _link->next)

/**
 * Initialize a list item.
 *
 * @param link The list item to initialize.
 */
static inline void link_init(llink_t* link) {
    assert(link != NULL);

    link->prev = NULL;
    link->next = NULL;
}

/**
 * Checked list initialization
 *
 * Initialize a list. An empty list points back
 * to itself for easy runtime checks.
 *
 * @param list The list head to initialize.
 */
static inline void llist_init(llist_t* list) {
    assert(list != NULL);

    list->head.prev = &list->head;
    list->head.next = &list->head;
}

/**
 * Test whether a list item is connected
 *
 * @param link The list item to examine.
 *
 * @return True if the list item is connected to a list.
 */
static inline bool link_is_connected(llink_t* link) {
    assert(link != NULL);

    return ((link->prev != NULL) && (link->next != NULL));
}

/**
 * Test whether a list is empty
 *
 * @param list The list to examine.
 * @return True if the list is empty.
 */
static inline bool llist_is_empty(llist_t* list) {
    assert(list != NULL);

    return (list->head.next == &list->head);
}

/**
 * Count the number of items in a list.
 *
 * @param list The list to examine.
 * @return Number of items in a list.
 */
static inline size_t llist_get_size(llist_t* list) {
    size_t result = 0;
    for (llink_t* link = list->head.next; link != &(list->head); link = link->next) {
        result++;
    }
    return result;
}

/**
 * Prepend a new item to the beginning of a list.
 *
 * @param list The list to prepend to.
 * @param link The new item link.
 */
static inline void llist_push_front(llist_t* list, llink_t* link) {
    assert(list != NULL);
    assert(link != NULL);

    link->next = list->head.next;
    link->prev = &list->head;

    list->head.next->prev = link;
    list->head.next = link;
}

/**
 * Append a new item to the end of a list.
 *
 * @param list The list to append to.
 * @param link The new item link.
 */
static inline void llist_push_back(llist_t* list, llink_t* link) {
    assert(list != NULL);
    assert(link != NULL);

    link->next = &list->head;
    link->prev = list->head.prev;

    list->head.prev->next = link;
    list->head.prev = link;
}

/**
 * Remove an item from any list it is currently part of.
 *
 * @param link The item link to remove.
 */
static inline void llist_remove(llink_t* link) {
    assert(link != NULL);

    if (link_is_connected(link)) {
        link->next->prev = link->prev;
        link->prev->next = link->next;
    }

    link_init(link);
}

/**
 * Remove the first item from the list.
 *
 * @param list The list to pop from.
 * @return The first item.
 */
static inline llink_t* llist_pop_front(llist_t* list) {
    assert(list != NULL);

    if (llist_is_empty(list))
        return NULL;

    llink_t* item = list->head.next;

    llist_remove(item);
    return item;
}

/**
 * Remove the last item from the list.
 *
 * @param list The list to pop from.
 * @return The last item.
 */
static inline llink_t* llist_pop_back(llist_t* list) {
    assert(list != NULL);

    if (llist_is_empty(list))
        return NULL;

    llink_t* item = list->head.prev;

    llist_remove(item);
    return item;
}

/**
 * Rotate the list by moving its head into its tail
 *
 * @param list The list to rotate.
 * @return The rotated item.
 */
static inline llink_t* llist_rotate(llist_t* list) {
    assert(list != NULL);

    if (llist_is_empty(list))
        return NULL;

    llink_t* item = llist_pop_front(list);
    llist_push_back(list, item);

    return item;
}

#endif