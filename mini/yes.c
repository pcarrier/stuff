/**
 * A simple (non-standards-compliants?) 'yes' implementation for fun
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EVER ;;

int main(int argc, char **argv)
{
    int current_arg;

    if (argc < 2)
        for (EVER)
            puts("y");

    else
        for (EVER) {
            for (current_arg = 1; current_arg < argc - 1; current_arg++) {
                fputs(argv[current_arg], stdout);
                putchar(' ');
            }
            puts(argv[argc - 1]);
        }

    return EXIT_FAILURE;        /* should never happen */
}
