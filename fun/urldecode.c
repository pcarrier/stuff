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
    char *decoded;
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
            decoded = g_uri_unescape_string(line, NULL);
            if (EOF == puts(decoded))
                return EXIT_FAILURE;
            free(line);
            free(decoded);
            line = NULL;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            decoded = g_uri_unescape_string(argv[i], NULL);
            if (EOF == puts(decoded))
                return EXIT_FAILURE;
            free(decoded);
        }
    }

    return EXIT_SUCCESS;
}
