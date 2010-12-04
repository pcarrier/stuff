#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EVER ;;

int main(int argc, char **argv)
{
    char *msg = "y";
    if (argc > 1)
        msg = argv[1];
    for (EVER)
        puts(msg);
    return EXIT_FAILURE;        /* should never happen */
}
