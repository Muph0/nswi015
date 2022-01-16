#include "test.h"
#include "struct.h"

typedef struct {

    int first;
    int second;
    int third;

} test_struct_t;

void test_main(void) {
    test_start("struct/offset");

    test_assert(struct_offset_of(test_struct_t,first) == 0 * sizeof(int));
    test_assert(struct_offset_of(test_struct_t,second) == 1 * sizeof(int));
    test_assert(struct_offset_of(test_struct_t,third) == 2 * sizeof(int));

    test_passed();
}