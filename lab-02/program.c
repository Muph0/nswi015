
#include "libmin.h"
#include <stdio.h>

int main(void) {

    int seed = 12345;
    const int size = 100;

    int array[size];

    for (int i = 0; i < size; i++) {
        array[i] = seed % 100;

        seed = seed << 5 - seed + i;
    }

    printf("The minimum is %d", min(array, size));
}
