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
 * and writes an random integer every 2KB to make sure it actually takes memory.
 *
 * Using stderr because it's unbuffered by default. Yes that's ugly.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define MB ((size_t)1<<20)

int main(int argc, char **argv)
{
	size_t buf_size, done;
    char *buf, *dark_passenger;
	long int nr_of_mb = 8192;
	if(argc > 1)
		nr_of_mb = (size_t)atoi(argv[1]);
	if(nr_of_mb <= 0)
		errx(1, "The memory size has to be positive");
	else fprintf(stderr,
		"We will really try to take those %li MB of memory, "
		"confirm with 'y'! ", nr_of_mb);
	
	if((char)getchar() != 'y')
		errx(2, "Operation cancelled");
	
	buf_size = (size_t)nr_of_mb * MB;
    buf = malloc(buf_size);
    if (buf == NULL)
        errx(3, "Could not allocate");

    for (dark_passenger = buf, done = 0; dark_passenger < buf + buf_size;
         dark_passenger += 2048, done += 2048) {
        *((int *) dark_passenger) = rand();
		if((size_t)(dark_passenger) % MB == 0) {
			if(done % (256*MB) == 0) {
				fprintf(stderr, "\n%li MB written.\n", done / MB);
			}
			fputc('.', stderr);
		}
    }

    fputs("\nCompletely done, going to sleep by SIGSTOP'ing myself.\n"
			"Remember to kill or SIGCONT me ('fg' from invoking shell) "
			"as I keep the memory.\n", stderr);
    kill(getpid(), SIGSTOP);
    return EXIT_SUCCESS;
}
