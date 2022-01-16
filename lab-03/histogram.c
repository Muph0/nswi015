
#include <stdio.h>
#include <stdlib.h>

const int WIDTH = 80;

int main(int argc, char** argv) {

    int numbers_count = 0;
    int* numbers = calloc(argc, sizeof(int));
    int min = __INT_MAX__, max = -__INT_MAX__;

    for (int i = 0; i < argc; i++) {
        numbers[i] = atoi(argv[i]);
        if (numbers[i] > max) max = numbers[i];
        if (numbers[i] < min) min = numbers[i];
        numbers_count++;
    }

    int range = max - min;
    int bucketCount = range > WIDTH ? WIDTH : range;
    int bucketSize = range / bucketCount;

    int* buckets = calloc(bucketCount, sizeof(int));
    for (int i = 0; i < numbers_count; i++) { }
}
