#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define FOO (2*1024*1024*1024)
#define BAR (1<<31)

int main()
{
    void *ptr;
    ptr = malloc(FOO);
    assert(ptr != NULL);
    ptr = malloc(BAR);
    assert(ptr != NULL);
    return EXIT_SUCCESS;
}
