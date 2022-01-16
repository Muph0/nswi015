#include "test.h"
#include "alist.h"

#define TEST_CAP 1

void test_main(void) {
    test_start("alist/w1r1");

    alist_t list = alist_create(TEST_CAP, sizeof(int));

    while (alist_count(list) < 10) {
        alist_append(list, int, 0);
    }

    for (int i = 0; i < (int)alist_count(list); i++) {
        alist_ref(list, int, i) = i;
    }

    for (int i = 0; i < (int)alist_count(list); i++) {
        test_assert(alist_ref(list, int, i) == i);
    }

    alist_destroy(list);
    test_passed();
}
