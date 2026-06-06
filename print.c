#include "print.h"
#include "gcprint.h"
#include <stdio.h>
#include <stdbool.h>
#include <gc.h>

int main(int argc, char *argv[]) {
    GC_INIT();

    print("My number is ", 1234, " (in hex: ", hex(1234), ") and a float is ", 2.3);

    const char *s = String("My number is ", 1234, " (in hex: ", hex(1234), ") and a float is ", 2.3);
    printf("String = \"%s\"\n", s);
    return 0;
}
