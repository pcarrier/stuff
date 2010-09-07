/*
Double check (allocate, write to, read from, release) a 256MB HugePages shm segment

Copyright (c) 2010, Red Hat Inc.
Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided that
the above copyright notice and this permission notice appear in all copies.

Maintainer: Pierre Carrier <prc@redhat.com>

This code relying on SHM_HUGETLB, it is Linux-specific.
Doesn't build with gcc -ansi.
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

/* 256MB */
#define LENGTH (256UL*1024*1024)

/* Only ia64 requires this */
#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define SHMAT_FLAGS (SHM_RND)
#else
#define ADDR (void *)(0x0UL)
#define SHMAT_FLAGS (0)
#endif

int main(void)
{
    int shmid;
    unsigned long i;
    char *shmaddr;

    printf("Getting a shared memory segment\n");
    if ((shmid =
	 shmget(2, LENGTH, SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W)) < 0) {
	perror("Getting failed!");
	exit(1);
    }
    printf("shmid: 0x%x\n", shmid);

    shmaddr = shmat(shmid, ADDR, SHMAT_FLAGS);
    if (shmaddr == (char *) -1) {
	perror("Attaching failed!");
	shmctl(shmid, IPC_RMID, NULL);
	exit(2);
    }
    printf("shmaddr: %p\n", shmaddr);

    printf("Writing:\n");
    for (i = 0; i < LENGTH; i++) {
	shmaddr[i] = (char) (i);
	if (!(i % (1024 * 1024)))
	    printf(".");
    }
    printf("\n");

    printf("Checking:");
    for (i = 0; i < LENGTH; i++)
	if (shmaddr[i] != (char) i)
	    printf("\nIndex %lu mismatched\n", i);
    printf("Done.\n");

    printf("Detaching the process from the shared memory segment...");
    if (shmdt((const void *) shmaddr) != 0) {
	perror("Detach failure");
	shmctl(shmid, IPC_RMID, NULL);
	exit(3);
    }

    printf("Deleting the shared memory segment.\n");
    shmctl(shmid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
