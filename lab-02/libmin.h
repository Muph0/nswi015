
#ifndef _LIBMIN_H
#define _LIBMIN_H

#include <sys/types.h>

/**
 * Find the minimum from an array of values.
 *
 * @param a The array to search in.
 * @param len The length of the array a.
 */
int min(int a[], ssize_t len);

#endif