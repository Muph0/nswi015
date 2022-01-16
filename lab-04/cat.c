
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096

void echo_file(int infile, int outfile) {
    char buf[BUF_SIZE];

    int rbytes = 0;
    while ((rbytes = read(infile, buf, sizeof(buf))) != 0) {
        if (rbytes == -1) {
            close(infile);
            err(1, "read");
        }

        if (write(outfile, buf, rbytes) == -1) {
            err(1, "error writing to fd %d", outfile);
        }
    }
}

int main(int argc, char** argv) {

    int infile = 0;
    int outfile = 1;

    if (argc == 1) {
        echo_file(infile, outfile);
    } else {
        for (int i = 1; i < argc; i++) {
            if (strcmp("--", argv[i]) != 0) {
                infile = open(argv[1], O_RDONLY);
            } else {
                infile = 0;
            }

            if (infile == -1) {
                err(1, "open");
            }

            echo_file(infile, outfile);
        }
    }
}
