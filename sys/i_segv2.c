/*
I segfault (faster)

Copyright (c) 2010, Red Hat Inc.
Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided that
the above copyright notice and this permission notice appear in all copies.

Maintainer: Pierre Carrier <prc@redhat.com>
*/

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
	kill(getpid(), SIGSEGV);
	return EXIT_FAILURE; /* Whaaat? */
}
