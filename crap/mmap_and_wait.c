#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


int main(int argc, char **argv)
{
    int file;
    char *fn;
    if (argc != 2)
        goto err;
fn = argv[1];
    file = open(fn, O_RDWR | O_CREAT, (mode_t) 0600);
    mmap(NULL, 1024 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED,
         file, (off_t) NULL);
    getchar();
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
