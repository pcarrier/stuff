/**
 * memcpy2memmove: if your app relies on memcpy for overlapping memory zones,
 * fix it. If impossible, we can help.
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Compile with:
 * gcc -g -shared -fPIC -ldl -o /usr/local/lib/memcpy2memmove.so memcpy2memmove.c
 * Use in a shell with:
 * LD_PRELOAD=/usr/local/lib/memcpy2memmove.so foo bar
 *
 * Warning: This will impact performance!
**/

#define _GNU_SOURCE 1
#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
    return(memmove(dest, src, n));
}
