
#include "libmin.h"

int min(int a[], ssize_t len) {
    int result = a[0];
    for (int i = 1; i < len; i++) {
        result = a[i] < result ? a[i] : result;
    }
}
