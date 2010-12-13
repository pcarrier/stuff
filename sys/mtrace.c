/**
 * mtrace.c - LD_PRELOAD library for malloc() debugging with glibc
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Compilation:
 * 	With older glibc versions:
 *	$ gcc -o mtrace.so -shared -fPIC -nostartfiles -DOLD_VERSION mtrace.c
 *
 *	With newer glibc versions:
 *	$ gcc -o mtrace.so -shared -fPIC mtrace.c
 *
 * Usage:
 * 	$ MALLOC_TRACE=./mtrace.log LD_PRELOAD=./mtrace.so PROGRAM_TO_DEBUG
 *	This will create a mtrace.log file containing informations about each
 *	call to malloc() and free() in the program that is run.
 *
 * See also ltrace(1), malloc(3), mtrace(3), valgrind(1).
 *
 * TODO: find out from what version we can use constructor
 *       tested to work with -DNEWER_VERSION on
 *       	rhel5 i386 glibc-2.5-34
 *       	rhel4 i386 glibc-2.3.4-2.41
**/

#include <mcheck.h>

#ifdef OLD_VERSION
void _init(void)
#else
__attribute__ ((constructor))
void init(void)
#endif
{
    mtrace();
}
