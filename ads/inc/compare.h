
#ifndef _DATA_COMPARE
#define _DATA_COMPARE

/**
 * Construct a function pointer declaration to a comparison function.
 * Comparison function is a function which defines an ordering on a type.
 * It takes two arguments of type T and returns:
 *      0 if they are equal
 *      1 if they are ascending
 *     -1 if they are descending
 *
 * @param T The type to compare.
 * @param name The declaration name.
 */
#define comparison_t(T, name) int (*name)(const T, const T)

/**
 * Test the result of a comparison with C operator.
 *
 * @param comparison A comparison function.
 * @param A The left operand.
 * @param op The operator. Must be one of <,>,<=,>=,==.
 * @param B The right operand.
 */
#define compare(comparison, A, op, B) (0 op((comparison)((A), (B))))

typedef comparison_t(int, comparison_default_t);

/**
 * Default comparison of two integers.
 *
 * @param A The left operand
 * @param B The right operand
 * @return When a < b return 1, when a > b return -1, when a == b return 0.
 */
static inline int comparison_default(const int A, const int B) {
    return (A < B) | -(A > B);
}

#endif
