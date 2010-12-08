/*
 * A simple 'logname' implementation for fun
**/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    char *login = getlogin();
    if (login == NULL || puts(login) == EOF || fclose(stdout))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
