#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define S1 (2*1024*1024*1024)
#define S2 (1<<31)
#define S3 2147483648

int main()
{
    void *ptr1, *ptr2, *ptr3;
    ptr1 = malloc(S1*sizeof(char)); /* Yes the sizeof is useless */
    ptr2 = malloc(S2*sizeof(char));
    ptr3 = malloc(S3*sizeof(char));
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr3 != NULL);
    return EXIT_SUCCESS;
}

/* Can someone explain:
 *
 * 1) Come, those HAVE to be equal.
 * In Python:
 * >>> 2*1024*1024*1024
 * 2147483648
 * >>> 1<<31
 * 2147483648
 *
 * 2) Why does one and one only get a warning?
 * $ gcc -Wall -Wextra -pedantic -o crap/weird_gcc.portable crap/weird_gcc.c -std=c99 -D_XOPEN_SOURCE
 * crap/weird_gcc.c: In function ‘main’:
 * crap/weird_gcc.c:12: warning: integer overflow in expression
 *
 * 3) Why does two and two only get crazy in ltrace?
 * malloc(18446744071562067968) = NULL
 * malloc(18446744071562067968) = NULL
 * malloc(2147483648)           = 0x7f0643fff010
*/
