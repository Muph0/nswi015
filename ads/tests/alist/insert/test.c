#include "test.h"
#include "alist.h"

#include <stdlib.h>

#define TEST_CAP 1
#define TEST_ITEMS 99
#define TEST_MID 50
#define TEST_MAGIC 123456789

void test_main(void) {
    test_start(__FILE__);

    alist_t list = alist_create(TEST_CAP, sizeof(int));

    for (int i = 0; i < TEST_ITEMS; i++) {
        alist_append(list, int, i);
    }

    alist_insert(list, int, TEST_MID) = TEST_MAGIC;
    test_assert(alist_count(list) == TEST_ITEMS + 1);

    for (int i = 0; i < alist_count(list); i++) {
        if (i < TEST_MID)
            test_assert(alist_ref(list, int, i) == i);
        else if (i > TEST_MID)
            test_assert(alist_ref(list, int, i) == i - 1);
        else
            test_assert(alist_ref(list, int, i) == TEST_MAGIC);
    }

    alist_destroy(list);
    test_passed();
}
