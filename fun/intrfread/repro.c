#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

void hello(int sig)
{
    fprintf(stderr, "Hello signal %s!", strsignal(sig));
}

int main(int argc, char **argv)
{
    char *path;
    size_t elems, elemsize, rsize;
    useconds_t delay;
    void *buff;
    FILE *fd;
    signal(SIGALRM, hello);
    if (argc != 5) {
	printf("Usage: %s path elems elemsize delay", argv[0]);
	return EXIT_FAILURE;
    }
    path = argv[1];
    elems = (size_t) atoi(argv[2]);
    assert(elems > 0);
    elemsize = (size_t) atoi(argv[3]);
    assert(elemsize > 0);
    delay = (useconds_t) atoi(argv[4]);
    assert(delay > 0);
    buff = malloc(elems * elemsize);
    assert(buff != NULL);
    fd = fopen(path, "r");
    assert(fd != NULL);
    ualarm(delay, 0);
    err = EINTR;
    printf("Reading...\n");
#ifdef ROTTEN
    rsize = fread(buff, elemsize, elems, fd);
    assert(rsize > 0);
#else
    do {
	rsize = fread(buff, elemsize, elems, fd);
    } while (ferror(fd) && errno == EINTR);
#endif
    printf("read %i = %i * %i\n", (int) rsize, (int) elems,
	   (int) elemsize);
    return EXIT_SUCCESS;
}
