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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <err.h>

#define MB ((size_t)1<<20)

int main(int argc, char **argv)
{
    size_t page_size, buf_size, done;
    char *buf, *dark_passenger;
    long int nr_of_mb = 8192;

    if (argc > 1)
        nr_of_mb = (size_t) atoi(argv[1]);

    if (nr_of_mb <= 0)
        errx(1, "Negative memory size: %li MB", nr_of_mb);
    else
        fprintf(stderr,
                "We will really try to take those %li MB of memory, "
                "confirm with 'y'! ", nr_of_mb);

    if ((char) getchar() != 'y')
        errx(2, "Operation cancelled");

    buf_size = (size_t) nr_of_mb *MB;
    buf = malloc(buf_size);
    if (buf == NULL)
        errx(3, "Could not allocate");

    page_size = (size_t) sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
        errx(4, "Wrong page size: %li bytes", page_size);

    for (dark_passenger = buf, done = 0; dark_passenger < buf + buf_size;
         dark_passenger += page_size, done += page_size) {
        *((int *) dark_passenger) = rand();
        if (done % (10 * MB) == 0) {
            if (done % (100 * MB) == 0)
                fprintf(stderr, " %li MB ", done / MB);
            else
                fputc('.', stderr);
        }
    }

    fputs("\nCompletely done, going to sleep by SIGSTOP'ing myself.\n"
          "Remember to kill or SIGCONT me ('fg' from invoking shell).\n",
          stderr);
    kill(getpid(), SIGSTOP);
    return EXIT_SUCCESS;
}
