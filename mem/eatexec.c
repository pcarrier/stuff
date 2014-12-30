/**
 * Allocate and write on loads of memory, then fork and wait a bunch of times :)
 *
 * Copyright (c) 2010, Red Hat Inc.
 *               2014, Twitter, Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <pcarrier@twitter.com>
 *
 * Usage: eatexec [mb] [execs] [binpath]
 * Allocates mb MBs of memory (8GB by default),
 * and writes an random integer in every page to make sure it actually takes memory.
 * Shows progress with a '.' every 10MB replaced by the amount reached every 100MB.
 * Then executes binpath (defaults to /bin/true) execs times, waiting on the child process
 * every time.
 *
 * Using stderr because it's unbuffered by default. Yes that's ugly.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <err.h>
#include <sysexits.h>

#define MB ((size_t)1<<20)

void print_time() {
    struct timeval tp;
    if (gettimeofday(&tp, NULL) < 0)
        errx(EX_OSERR, "Could not get time");
    fprintf(stderr, "\nTime: %li %06li\n", (long int) tp.tv_sec, (long int) tp.tv_usec);
}

int main(int argc, char **argv)
{
    size_t page_size, buf_size, done, nr_of_mb = 8192, nr_of_forks = 1000;
    char *buf, *dark_passenger;
    pid_t pid;
    int stat;
    char *cmd = "/bin/true";

    if (argc > 1)
        nr_of_mb = (size_t) atoi(argv[1]);

    if (argc > 2)
        nr_of_forks = (size_t) atoi(argv[2]);

    if (argc > 3)
        cmd = argv[3];

    if (nr_of_mb <= 0)
        errx(EX_USAGE, "Negative memory size: %li MB", nr_of_mb);
    else if (nr_of_forks <= 0)
        errx(EX_USAGE, "Negative number of forks: %li", nr_of_forks);
    else
        fprintf(stderr,
                "We will really try to take those %li MB of memory then fork %li times, "
                "confirm with 'y'! ", nr_of_mb, nr_of_forks);

    if ((char) getchar() != 'y')
        errx(1, "Operation cancelled");

    buf_size = (size_t) nr_of_mb *MB;
    buf = malloc(buf_size);
    if (buf == NULL)
        errx(EX_OSERR, "Could not allocate");

    page_size = (size_t) sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
        errx(EX_OSERR, "Wrong page size: %li bytes", (long int) page_size);

    for (dark_passenger = buf, done = 0; dark_passenger < buf + buf_size;
         dark_passenger += page_size, done += page_size) {
        *((int *) dark_passenger) = rand();
        if (done % (10 * MB) == 0) {
            if (done % (100 * MB) == 0)
                fprintf(stderr, " %li MB ", (long int) (done / MB));
            else
                fputc('.', stderr);
        }
    }

    print_time();

    for (done = 0; done < nr_of_forks; done++) {
        pid = fork();
        if (pid < 0)
            errx(EX_OSERR, "Couldn't fork");
        else if (pid == (pid_t) 0) {
            execv(cmd, (char*[]) {cmd, NULL});
            errx(EX_OSERR, "Didn't exec %s", cmd);
        } else {
            if (wait(&stat) < 0)
                errx(EX_OSERR, "Waiting failed");
        }

        if (done % 10 == 0)
            fprintf(stderr, " %li ", (long int) done);
        else
            fputc('.', stderr);
    }

    print_time();

    return EX_OK;
}
