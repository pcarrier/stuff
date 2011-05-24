#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    int ret, fd_flags;
    char buff[1024];
    size_t len;

    struct pollfd fds;
    fds.fd = 0;
    fds.events = POLLIN;

    if((fd_flags = fcntl(0, F_GETFL)) < 0)
            err(1, "F_GETFL");
    if(fcntl(0, F_SETFD, fd_flags | O_NONBLOCK) < 0)
            err(1, "F_SETFD");

    for (;;) {
        ret = poll(&fds, 1, 1000);
        if (ret < 0) {
            perror("poll failed");
            exit(-1);
        } else if (ret == 0) {
            fputs("nothing new!\n", stderr);
        } else {
            len = read(0, buff, 1024);
            buff[len + 1] = '\0';
            printf("read %i bytes: \"%s\"\n", (int) len, buff);
        }
    }
    return EXIT_SUCCESS;
}
