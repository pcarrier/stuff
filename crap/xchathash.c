/*
 * This is a stupid string hasher for X-Chat and X-Chat Gnome development.
 * There's probably a simpler way, like an existing plugin,
 * but I haven't found it.
 *
 * Turns out they use hardcoded hashes everywhere in the code,
 * so use './xchathash foo' to find the string to look for
 * if you're interested in 'foo'.
 *
 * You might want to check that str_hash wasn't modified first,
 * but I doubt they'll ever go through the pain of changing all their
 * magical numbers.
 *
 * By the way, F U X-Chat.
 * Ever heard of #define?
 */

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

guint32
str_hash (const char *key)
{
    const char *p = key;
    guint32 h = *p;
    if (h)
        for (p += 1; *p != '\0'; p++)
            h = (h << 5) - h + *p;
    return h;
}

int main(int argc, char **argv) {
    if (argc != 2)
        return EXIT_FAILURE;

    printf("%#x\n", str_hash(argv[1]));

    return EXIT_SUCCESS;
}
