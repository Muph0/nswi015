#include <math.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>

static const int LINE_L = 80;

int main(void) {

    int pos = 0;
    bool forward = true;

    int i = 0;


    while (i++ < LINE_L * 10) {
        putchar('\r');
        printf("\e[%dC", pos);

        if (pos == LINE_L) forward = false;
        if (pos == 0) forward = true;

        pos += forward ? 1 : -1;

        printf("\e[2K*\e[1G");
        fflush(stdout);
        poll(NULL, 0, 20);
    }
}
