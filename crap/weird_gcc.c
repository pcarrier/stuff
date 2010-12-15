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

/* Can someone explain:
 * crap/weird_gcc.c: In function ‘main’:
 * crap/weird_gcc.c:11: warning: integer overflow in expression
*/
