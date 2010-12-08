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
    size_t len = 0;
    char *msg = NULL, *cur_pos;
    if (argc < 2)
        for (EVER) {
            if (puts("y") == EOF)
                goto err;
        }

    else {
        len = argc - 1;

        for (current_arg = 1; current_arg < argc; current_arg++)
            len += strlen(argv[current_arg]);

        if ((msg = malloc(len)) == NULL)
            goto err;

        cur_pos = msg;
        for (current_arg = 1; current_arg < argc; current_arg++) {
            len = strlen(argv[current_arg]);
            memcpy(cur_pos, argv[current_arg], len);
            cur_pos[len] = ' ';
            cur_pos += len + 1;
        }
        cur_pos[-1] = '\0';

        for (EVER)
            if (puts(msg) == EOF)
                goto err;

        free(msg);              /* should never happen */
    }

  err:
    return EXIT_FAILURE;        /* either */
}
