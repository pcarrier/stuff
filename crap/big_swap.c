#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE ((size_t)1<<33) /* 8GB */

int main()
{
    char *buf, *dark_passenger;
    buf = malloc(BUFSIZE);
    if (buf == NULL)
        goto err;
    for (dark_passenger = buf; dark_passenger < buf + BUFSIZE;
         dark_passenger += 2048) {
        *((int *) dark_passenger) = rand();
        fputs(".", stderr);
    }
    fputs(" done.\n", stderr);
    getchar();
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
