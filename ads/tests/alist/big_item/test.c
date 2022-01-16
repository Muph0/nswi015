#include "test.h"
#include "alist.h"
#include "alist_internal.h"

#include <stdlib.h>

#define TEST_CAP 1
#define TEST_ITEMS 10

typedef struct
{
    size_t a;
    size_t b;
    size_t c;
} data_t;

void test_main(void) {
    test_start(__FILE__);

    alist_t list = alist_create(TEST_CAP, sizeof(data_t));
    data_t* ref_list = calloc(TEST_ITEMS, sizeof(data_t));

    test_assert(list->itemSize == sizeof(data_t));

    for (int i = 0; i < TEST_ITEMS; i++) {
        data_t data = {
            .a = rand(),
            .b = rand(),
            .c = rand(),
        };

        alist_append(list, data_t, data);
        ref_list[i] = data;
    }

    for (int i = 0; i < TEST_ITEMS; i++) {
        data_t left, right;
        left = ref_list[i];
        right = alist_ref(list, data_t, i);

        printf("[%d] == {\n  .a = 0x%lx,\n  .b = 0x%lx,\n  .c = 0x%lx\n}\n", i, left.a, left.b, left.c);

        test_assert(0 == memcmp(&left, &right, sizeof(data_t)));
    }

    alist_destroy(list);
    test_passed();
}
