#include "test.h"
#include "alist.h"

#define TEST_CAP 1
#define TEST_ITEMS 10

void test_main(void) {
    test_start(__FILE__);

    alist_t list = alist_create(TEST_CAP, sizeof(int));

    for (int i = 0; i < TEST_ITEMS; i++) {
        alist_append(list, int, 123456789);
    }

    test_assert(alist_count(list) == TEST_ITEMS);

    alist_destroy(list);

    test_passed();
}
