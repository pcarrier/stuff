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

#define _BSD_SOURCE

#ifdef __APPLE__
# define _DARWIN_C_SOURCE
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <string.h>             /* required by strerror */

int main(int argc, char **argv)
{
    unsigned int length = 0, new_hostid;
    long old_hostid = gethostid();

    if (argc != 2)
        goto err;

    length = strlen(argv[1]);

    if (length != 8 || sscanf(argv[1], "%x", &new_hostid) != 1)
        goto err;

    if ((long) new_hostid == old_hostid)
        return EXIT_SUCCESS;

    if (sethostid(new_hostid) < 0) {
        fprintf(stderr, "Failed with %u (%s)\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

  err:
    errx(EXIT_FAILURE, "Usage: %s id\n"
            "id is an 8-char hexadecimal representation, "
            "as in the output of 'hostid'.\n", argv[0]);
}
