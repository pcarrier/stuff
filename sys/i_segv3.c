/**
 * I segfault (multithreaded)
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

void *blurp(void *data)
{
    sleep(3600);
    return data;
}

int main()
{
    pthread_t thread0, thread1;
    if (pthread_create(&thread0, NULL, blurp, NULL) ||
        pthread_create(&thread1, NULL, blurp, NULL))
        return EXIT_FAILURE;
    if (pthread_kill(thread0, SIGSEGV) || pthread_kill(thread1, SIGSEGV))
        return -errno;
    if (pthread_join(thread0, NULL) || pthread_join(thread1, NULL))
        return errno;
    return EXIT_SUCCESS;        /* Whaaat? */
}
