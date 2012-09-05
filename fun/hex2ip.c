#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    unsigned int a, b, c, d;

    if (argc != 2)
        return EXIT_FAILURE;

    if (strlen(argv[1]) != 8)
        return EXIT_FAILURE;

    if (sscanf(argv[1], "%2X%2X%2X%2X", &a, &b, &c, &d) != 4)
        return EXIT_FAILURE;

    if (printf("%u.%u.%u.%u\n", a, b, c, d) < 7)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
