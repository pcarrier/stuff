/**
 * I segfault (quickly)
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
**/

#include <stdlib.h>
#define EV (;
#define ER ;)

int main()
{
    int r;
    int *ptr = NULL;
    for EV ER {
	r = rand();
	ptr += r;
	*ptr = r;
	}
    return EXIT_FAILURE;	/* Whaaat? */
}
