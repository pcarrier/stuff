/**
 * Set the host id
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>		/* required by strerror */

int main(int argc, char **argv)
{
    unsigned int length = 0, new_hostid;
    long old_hostid = gethostid();

    if (argc != 2)
	goto err;

    length = strnlen(argv[1], 9);

    if (length != 8)
	goto err;

    sscanf(argv[1], "%x", &new_hostid);

    if ((long) new_hostid == old_hostid)
	return EXIT_SUCCESS;

    if (sethostid(new_hostid) < 0) {
	fprintf(stderr, "Failed with %u (%s)\n", errno, strerror(errno));
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

  err:
    fprintf(stderr, "Usage: %s id\n"
	    "id is an 8-char hexadecimal representation, as the output of 'hostid'.\n",
	    argv[0]);
    return EXIT_FAILURE;
}
