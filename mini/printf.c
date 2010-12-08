/**
 * A simple printf implementation for fun
 *
 * Completely useless half of a skeleton at the moment.
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

static inline void escape(char *to, char *from)
{
    int backslashed, attempted_backslashed, max_remaining_digits;
    size_t jump;
    char *cur_from, *cur_to = to, *remaining = from;
    char *end_of_from = from + strlen(from);

    while (remaining < end_of_from) {
        cur_from = strchr(remaining, '\\');

        if (cur_from == NULL) {
            jump = end_of_from - remaining;
            memcpy(cur_to, remaining, jump);
            cur_to += jump;
            break;
        }
        // else
        jump = cur_from - remaining;
        memcpy(cur_to, remaining, jump);
        cur_from += jump;
        cur_to += jump;
#define ESCAPED_REGISTER(c, p) case c: *cur_to = p; ++cur_from; ++cur_to; break;
        switch (*(++cur_from)) {
            ESCAPED_REGISTER('\\', '\\');
            ESCAPED_REGISTER('a', '\a');
            ESCAPED_REGISTER('b', '\b');
            ESCAPED_REGISTER('e', '\e');
            ESCAPED_REGISTER('f', '\f');
            ESCAPED_REGISTER('n', '\n');
            ESCAPED_REGISTER('r', '\r');
            ESCAPED_REGISTER('t', '\t');
            ESCAPED_REGISTER('v', '\v');
            ESCAPED_REGISTER('\0', '\\');
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            for (backslashed = 0, attempted_backslashed =
                 0, max_remaining_digits = 3;
                 max_remaining_digits > 0 && *cur_from >= '0'
                 && *cur_from <= '7'; cur_from++, max_remaining_digits--) {
                attempted_backslashed *= 8;
                attempted_backslashed += (*cur_from - '0');
                if (attempted_backslashed <= 0xff)
                    backslashed = attempted_backslashed;
                else
                    break;
            }
            *cur_to++ = backslashed;
            break;
        case 'x':
            for (backslashed = 0, attempted_backslashed =
                 0, max_remaining_digits = 2, cur_from++;
                 max_remaining_digits > 0
                 && ((*cur_from >= '0' && *cur_from <= '9')
                     || (*cur_from >= 'a' && *cur_from <= 'f')
                     || (*cur_from >= 'A' && *cur_from <= 'F'));
                 cur_from++, max_remaining_digits--) {
                attempted_backslashed *= 16;
                if (*cur_from >= 'a')
                    attempted_backslashed += 10 + (*cur_from - 'a');
                else if (*cur_from >= 'A')
                    attempted_backslashed += 10 + (*cur_from - 'A');
                else
                    attempted_backslashed += (*cur_from - '0');
                if (attempted_backslashed <= 0xff)
                    backslashed = attempted_backslashed;
                else
                    break;
            }
            *cur_to++ = backslashed;
            break;
        }
        remaining = cur_from;
    }
    *cur_to = '\0';
}

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
