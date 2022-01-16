#include "test.h"
#include "alist.h"

#define TEST_CAP 1

static char* strings[] = {
    "Hello",
    "this",
    "is",
    "",
    "a",
    "test",
    "123456789",
    NULL
};

void test_main(void) {
    test_start(__FILE__);
    alist_t list = alist_create(TEST_CAP, sizeof(char*));

    for (int i = 0; strings[i]; i++) {
        alist_append(list, char*, strings[i]);
    }

    char** values = &alist_ref(list, char*, 0);

    for (int i = 0; strings[i]; i++) {
        char* real = values[i];
        char* expected = strings[i];
        test_assert(strcmp(expected, real) == 0);
    }

    alist_destroy(list);
    test_passed();
}
