/*
 * A simple 'hostid' implementation for fun
**/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("%08x\n", (int) gethostid());
    if (fclose(stdout))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
