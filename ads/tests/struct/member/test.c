#include "test.h"
#include "struct.h"

typedef struct {
    int first;
    char test;
    int second;
    char buf[23];
    int third;
} test_t;

void test_main(void) {
    test_start("struct/member");

    test_t obj;

    int* first_p = &obj.first;
    int* second_p = &obj.second;
    int* third_p = &obj.third;

    test_assert(struct_container_of(first_p, test_t, first) == &obj);
    test_assert(struct_container_of(second_p, test_t, second) == &obj);
    test_assert(struct_container_of(third_p, test_t, third) == &obj);

    test_passed();
}
