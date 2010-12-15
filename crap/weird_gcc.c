#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define FOO (2*1024*1024*1024)
#define BAR (1<<31)

int main()
{
    void *ptr1, *ptr2;
    ptr1 = malloc(FOO*sizeof(char));
    ptr2 = malloc(BAR*sizeof(char));
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    return EXIT_SUCCESS;
}

/* Can someone explain:
 *
 * crap/weird_gcc.c: In function ‘main’:
 * crap/weird_gcc.c:11: warning: integer overflow in expression
 *
 * >>> 2*1024*1024*1024
 * 2147483648
 * >>> 1<<31
 * 2147483648
 *
 * ltrace:
 * malloc(18446744071562067968) = NULL
 * malloc(18446744071562067968) = NULL
*/
