#include "test.h"
#include "alist.h"

#define TEST_CAP 1
#define TEST_ITEMS 10

void populate(alist_t list) {
    for (int i = 0; i < TEST_ITEMS; i++) {
        alist_push_back(list, int, i + TEST_ITEMS);
    }

    for (int i = TEST_ITEMS - 1; i >= 0; i--) {
        alist_push_front(list, int, i);
    }
}

void test_main(void) {
    test_start(__FILE__);
    alist_t list = alist_create(TEST_CAP, sizeof(int));

    populate(list);

    while (alist_count(list) > 0) {

        int *it = alist_end(list, int);
        alist_pop_back(list, int);

        if (it + 1 < alist_end(list, int))
            test_assert(it[0] < it[1]);
    }

    populate(list);

    alist_foreach(list, int, it) {
        if (it + 1 < alist_end(list, int))
            test_assert(it[0] < it[1]);
    }

    alist_destroy(list);
    test_passed();
}


