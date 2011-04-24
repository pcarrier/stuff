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

#define UNKNOWN "Unknown error"
#define STRINGIFY(S) #S

/*
while read errno; do
  echo -e "#ifdef $errno\n  [$errno] = \"$errno\",\n#endif"
done < errnos.list
 */

const char* const errconsts[] = {
#include "errnos.h"

#ifndef __MACH__
#ifdef EWOULDBLOCK
  [EWOULDBLOCK] = "EWOULDBLOCK",
#endif
#endif /* TARGET_OS_MAC */
};

int main()
{
    int errnr, errmax = 1024;
    const char *errstr = NULL;
	const char *errconst = NULL;
    for (errnr = 0; errnr <= errmax; errnr++) {
        errstr = strerror(errnr);
        if(strncmp(errstr, UNKNOWN, sizeof(UNKNOWN)-1)) /* Found one */
			errmax = errnr + 1024;
		else
			continue;
		
		errconst = errconsts[errnr];
		errconst = (errconst && *errconst) ? errconst : "?";

        printf("%4i\t0x%02x\t%s\t%s\n", errnr, errnr, errconst, errstr);
    }
	fprintf(stderr, "Stopped looking at %i\n", errnr);
	return fclose(stdout);
}
