#include "test.h"

#include <stdbool.h>

static bool _test_started = false;
bool test_started(void) { return _test_started; }

void test_start(const char* name) {
    test_assertf(_test_started == false, "Test started twice.");
    printf("== TEST %s ==\n", name);
    _test_started = true;
}

/**
 * Common entry point of all test programs.
 */
int main(void) {
    test_main();

    test_assertf(_test_started, "Test did not call test_start().");
    test_failed_with("Test did not call test_passed() nor test_failed().");
}
