#include "test.h"
#include "alist.h"
#include "alist_internal.h"

#include <malloc.h>

#define TEST_CAP 123

void test_main(void) {
    test_start("alist/create");

    alist_t list = alist_create(TEST_CAP, sizeof(int));

    test_assert(list->capacity == TEST_CAP);
    test_assert(list->contents != NULL);
    test_assert(malloc_usable_size(list->contents) >= TEST_CAP * sizeof(int));
    test_assert(list->count == 0);

    alist_destroy(list);

    test_passed();
}
