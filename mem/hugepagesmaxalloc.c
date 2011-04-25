/**
 * Maximum size available in HugePages
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * This code relying on SHM_HUGETLB, it is Linux-specific.
 * Doesn't build with gcc -ansi.
**/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>             /* needed by strerror() */
#include <errno.h>
#include <sys/shm.h>
#include <limits.h>

/* 1MB each */
#define PRECISION (unsigned long)(1<<20)
#define MB (unsigned long)(1<<20)

#ifndef SHM_HUGETLB
# define SHM_HUGETLB 04000
#endif

/* Only ia64 requires this */
#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define SHMAT_FLAGS (SHM_RND)
#else
#define ADDR (void *)(0x0UL)
#define SHMAT_FLAGS (0)
#endif


int main()
{
    int shmid;
    unsigned long succeeds = 0;
    unsigned long fails = ULONG_MAX;
    unsigned long size = fails / 2;

    printf("Scanning for the biggest HUGETLB segment size available.\n");
    while (fails - succeeds > PRECISION) {
        printf("Trying size %lu (%luMB),", size, size / MB);
        if ((shmid = shmget(2, size,
                            SHM_HUGETLB | IPC_CREAT | IPC_EXCL | 0600)) <
            0) {
            printf(" failed with %u (%s)\n", errno, strerror(errno));
            fails = size;
        } else {
            shmctl(shmid, IPC_RMID, NULL);
            printf(" succeeded\n");
            succeeds = size;
        }
        size = (fails + succeeds) / 2;
    }

    fprintf(stderr, "Max segment size is %lu (%luMB), +- %lu (%luMB)\n",
            size, size / MB, PRECISION, PRECISION / MB);
    return EXIT_SUCCESS;
}
