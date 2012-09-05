#include <glib.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{

    char *line = NULL;
    char *last;
    char *encoded;
    size_t length;

    if (NULL == setlocale(LC_ALL, "")) {
        fputs("setlocale failed\n", stderr);
        return EXIT_FAILURE;
    }

    if (argc <= 1) {
        while (getline(&line, &length, stdin) != -1) {
            last = line + strlen(line) - 1;
            if (*last == '\n')
                *last = '\0';
            encoded = g_uri_escape_string(line, NULL, FALSE);
            if (EOF == puts(encoded))
                return EXIT_FAILURE;
            free(line);
            free(encoded);
            line = NULL;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            encoded = g_uri_escape_string(argv[i], NULL, FALSE);
            if (EOF == puts(encoded))
                return EXIT_FAILURE;
            free(encoded);
        }
    }

    return EXIT_SUCCESS;
}
