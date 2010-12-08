/**
 * A simple printf implementation for fun
 *
 * Completely broken at the moment.
 *
 * TODO:
 * - \u and \U (refactoring needed, and more knowledge of the wchar_t magic)
 * - converting parameters accordingly
 * - calling printf() he he
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "escape.c"

int main(int argc, char **argv)
{
    int len;
    char *escaped = NULL;
    if (argc > 1) {
        len = strlen(argv[1]) + 1;
        escaped = malloc(len);
        if (escaped == NULL)
            goto err;
        escape(escaped, argv[1]);
/* That's where the magic is missing :)
 * Having to convert to the proper types
 * ourselves is a pain in the bottom */
        free(escaped);
    }
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
