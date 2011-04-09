#include <stdlib.h>
#include <err.h>
#include <sys/select.h>

int main(int argc, char **argv)
{
    fd_set readfds;
    struct timeval timeout;
    if (argc < 2)
        errx(1, "Give microsecs to wait for as an argument.");
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    timeout.tv_sec = 0L;
    timeout.tv_usec = atol(argv[1]);
    while (1) {
        select(1, &readfds, NULL, NULL, &timeout);
    }
    return EXIT_SUCCESS;
}
