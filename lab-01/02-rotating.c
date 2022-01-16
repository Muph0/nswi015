#include <stdio.h>
#include <poll.h>

int main(void) { //
    char spinner[] = { '/', '-', '\\', '|', };
    int frame = 0;

    while (1) {
        putchar('\r');
        putchar(spinner[frame++ % sizeof(spinner)]);
        fflush(stdout);
        poll(NULL, 0, 100);
    }
}
