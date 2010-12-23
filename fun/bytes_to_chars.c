/**
 * Show characters numbers (signed or not dep on the arch), and unsigned hex
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
**/

#include <stdlib.h>
#include <stdio.h>

int main()
{
    char c = '\0';
    int pos;
    for (pos = 0; pos < 256; pos++, c++)
        printf("%c\t%i\t0x%x\n", c, c, (unsigned char) c);
    return EXIT_SUCCESS;
}
