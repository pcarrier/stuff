/**
 * Allocate and write on loads of memory
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Usage: big_swap [n]
 * Allocates n MB of memory (8GB by default),
 * and writes an random integer in every page to make sure it actually takes memory.
 * Shows progress with a '.' every 10MB replaced by the amount reached every 100MB.
 *
 * Using stderr because it's unbuffered by default. Yes that's ugly.
 */

#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#define MB ((size_t)1<<20)

void print_time() {
    struct timeval tp;
    if (gettimeofday(&tp, NULL) < 0)
        errx(EX_OSERR, "Could not get time");
    fprintf(stderr, "\nTime: %li %06li\n", (long int) tp.tv_sec, (long int) tp.tv_usec);
}

#ifndef MCL_CURRENT
# define MCL_CURRENT 1
#endif
#ifndef MCL_FUTURE
# define MCL_FUTURE 2
#endif
#ifndef MCL_ONFAULT
# define MCL_ONFAULT 4
#endif


#ifndef __NR_mlock2
# define __NR_mlock2 284
#endif

int my_mlock2(const void *addr, size_t len, int flags) {
    return syscall(__NR_mlock2, addr, len, flags);
}

int main(int argc, char **argv)
{
    size_t page_size, buf_size, done;
    char *buf, *dark_passenger, *unbuf;
    long int wired = 8192, unwired = 8192;
    int flags = MCL_FUTURE;

    if (argc > 3)
        flags = (size_t) atoi(argv[3]);

    if (argc > 2)
        unwired = (size_t) atoi(argv[2]);

    if (argc > 1)
        wired = (size_t) atoi(argv[1]);

    if (wired <= 0)
        errx(EX_USAGE, "Negative wired size: %li MB", wired);
    else if (unwired <= 0)
        errx(EX_USAGE, "Negative unwired size: %li MB", wired);
    else
        fprintf(stderr,
                "We will really try to take those %li MB of memory, "
                "and allocate %li MB of unwired memory, "
                "confirm with 'y'! ", wired, unwired);

    if ((char) getchar() != 'y')
        errx(1, "Operation cancelled");

    print_time();

    buf_size = (size_t) wired *MB;
    buf = malloc(buf_size);
    if (buf == NULL)
        errx(EX_OSERR, "Could not allocate wired memory");

    unbuf = malloc(unwired *MB);
    if (unbuf == NULL)
        errx(EX_OSERR, "Could not allocate unwired memory");

    page_size = (size_t) sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
        errx(EX_OSERR, "Wrong page size: %li bytes", (long int) page_size);

    print_time();    

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

    if (fork() == 0) {
        print_time();
        my_mlock2(buf, buf_size, flags);
        print_time();
        kill(getpid(), SIGSTOP);
    } else {
        kill(getpid(), SIGSTOP);
    }
    return EX_OK;
}
