#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BUFSIZE (2*1024*1024*1024)

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
