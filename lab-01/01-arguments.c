#include <stdio.h>

#define main2 main

int main1(int argc, char** argv) {

    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    return 0;
}

int main2(int argc, char** argv) {
    char** arg = argv;
    while (arg < argv + argc) {
        printf("%s\n", *arg++);
    }

    return 0;
}

