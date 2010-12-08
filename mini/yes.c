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
        for (EVER) {
            if (puts("y") == EOF)
                goto err;
        }

    else
        for (EVER) {
            for (current_arg = 1; current_arg < argc - 1; current_arg++)
                if (fputs(argv[current_arg], stdout) == EOF
                    || putchar(' ') == EOF)
                    goto err;
            if (puts(argv[argc - 1]) == EOF)
                goto err;
        }

  err:
    return EXIT_FAILURE;        /* should never happen */
}
