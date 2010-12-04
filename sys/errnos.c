/**
 * errno strings
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int errnr, errmax = 0;
    if ((argc < 2) || (errmax = atoi(*(argv + 1))) <= 0) {
	fprintf(stderr,
		"Usage: %s errmax\n"
		"	errmax the maximum errno to go through\n"
		"	132 seems the max in Linux 2.6.35\n", *argv);
	return EXIT_FAILURE;
    }
    for (errnr = 0; errnr <= errmax; errnr++) {
	printf("%i: [%s]\n", errnr, strerror(errnr));
    }
    return EXIT_SUCCESS;
}
