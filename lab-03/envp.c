
#include <stdio.h>
#include <stdlib.h>

extern char** environ;

void print_env() {
    char** envp = environ;
    while (*envp != NULL) {
        printf("%s\n", *envp);
        envp++;
    }
}

int main(void) {

    printf("%p\n", environ);
    print_env();

    putenv("A=123");
    putenv("B=456");
    putenv("C=789");

    printf("%p\n", environ);
    print_env();
}
