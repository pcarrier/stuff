/*
 * A simple 'sync' implementation for fun
**/

#define _BSD_SOURCE

#include <stdlib.h>
#include <unistd.h>

int main()
{
    sync();
    return EXIT_SUCCESS;
}
