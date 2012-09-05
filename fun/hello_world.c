#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    if (0 >= fprintf(stdout, "Hello!\n")) {
        perror("fprintf");
        return EXIT_FAILURE;
    }
    if (0 != fclose(stdout)) {
        perror("fclose");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
