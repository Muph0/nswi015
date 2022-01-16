
#ifndef _DATA_TEST_H
#define _DATA_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUOTED_(x) #x
#define QUOTED(x) QUOTED_(x)

void test_start(const char* name);
bool test_started(void);

#define TEST_EXPECTED   "[EXPECT] "

#define test_start test_start

#define test_passed() \
    do { \
        puts("\n\nTest passed.\n\n"); \
        exit(0); \
    } while (0)

#define test_failed() \
    do { \
        puts("\n\nTest failed.\n\n"); \
        exit(1); \
    } while (0)

#define test_failed_with(msg) \
    do { \
        puts("\n\nTest failed: " msg "\n\n"); \
        exit(1); \
    } while (0)

#define test_assert(expr) \
    do { \
        if (!(expr)) { \
            puts("\n\n" __FILE__ ":" QUOTED(__LINE__) ": Test assertion failed: " #expr); \
            test_failed(); \
        } \
    } while (0)

#define test_assertf(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            puts("\n\n" __FILE__ ":" QUOTED(__LINE__) ": Test assertion failed: " #expr); \
            printf(__FILE__ ":" QUOTED(__LINE__) ": " fmt "\n", ##__VA_ARGS__); \
            test_failed(); \
        } \
    } while (0)

#define test_expect(printer, expected) \
    do { \
        puts(TEST_EXPECTED expected); \
        printer; \
    } while (0)

/**
 * Specific test entry point.
 */
void test_main(void);

#endif