#!/bin/sh
# YES, THAT'S SILENT DATA CORRUPTION.
cd $(mktemp -d)
cat > test.c << EOF
/*
Double check (allocate, write to, read from, release) a mmap'ed file behaviour

Copyright (c) 2010, Red Hat Inc.
Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided that
 the above copyright notice and this permission notice appear in all copies.

Maintainer: Pierre Carrier <prc@redhat.com>
*/

#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int test_open(char *fn, int pass)
{
    int file = open(fn, O_RDWR | O_CREAT, (mode_t) 0600);
    if (file < 0) {
	fprintf(stderr, "File %s, pass %i: ", fn, pass);
	perror("Error when opening");
	exit(EXIT_FAILURE);
    }
    return file;
}

void test_close(int file, int pass)
{
    if (close(file) < 0) {
	fprintf(stderr, "Pass %i: ", pass);
	perror("Error when closing the file descriptor");
	exit(EXIT_FAILURE);
    }
}

void test_sync(int file)
{
    if (fsync(file) < 0) {
	perror("Error when syncing");
	exit(EXIT_FAILURE);
    }
}

char *test_mmap(int file, int size)
{
    char *buff = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, file,
		      (off_t) 0);
    if (buff == MAP_FAILED) {
	perror("Error when mmapping");
	test_close(file, 0);
	exit(EXIT_FAILURE);
    }
    return buff;
}

void test_munmap(char *buff, int size)
{
    if (munmap(buff, size) == -1) {
	perror("Error un-mapping");
	exit(EXIT_FAILURE);
    }
}

void test_enlarge(int file, int size)
{
    off_t result = lseek(file, size - 1, SEEK_SET);
    if (result == (off_t) (-1)) {
	perror("Error when seeking");
	test_close(file, 0);
	exit(EXIT_FAILURE);
    }
    if (write(file, "", 1) != 1) {
	perror("Error when enlarging");
	test_close(file, 0);
	exit(EXIT_FAILURE);
    }
}

void test_fill(char *buff, int size)
{
    int i;
    printf("Writing:\n");
    for (i = 0; i < size; i++) {
	buff[i] = (char) (i % 256);
	if (!(i % (1024 * 1024)))
	    printf(".");
    }
    printf("\n");
}

void test_read(char *buff, int size)
{
    int i;
    printf("Checking:\n");
    for (i = 0; i < size; i++) {
	if (!(buff[i] == (char) (i % 256))) {
	    fprintf(stderr, "Memory check failed!\n");
	    exit(EXIT_FAILURE);
	}
	if (!(i % (1024 * 1024)))
	    printf(".");
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int size = 0, result, file, pass = 1;
    int create = 0, fill = 0, verify = 0, sync = 0;
    char *cmds, *cmd, *fn, *buff;
    /* Check params */
    if (argc != 4) {
	fprintf(stderr,
		"Usage: %s commands filename size\n"
		"\tcommand can contain [c]reate, [f]ill, [s]ync, [v]erify\n"
		"\tfilename the file to fill\n"
		"\tsize its future size\n", argv[0]);
	exit(EXIT_FAILURE);
    }
    cmds = argv[1];
    fn = argv[2];
    size = atoi(argv[3]);
    if (size <= 0) {
	fprintf(stderr,
		"The specified size, %s, is not a positive integer!\n",
		argv[2]);
	exit(EXIT_FAILURE);
    }

    for (cmd = cmds + strlen(cmds); cmd >= cmds; cmd--) {
	switch ((int) *cmd) {
	case (int) 'c':
	    create = 1;
	    break;
	case (int) 'f':
	    fill = 1;
	    break;
	case (int) 's':
	    sync = 1;
	    break;
	case (int) 'v':
	    verify = 1;
	    break;
	default:
	    break;
	}
    }

    if (create) {
	printf("Creating.\n");
	file = test_open(fn, pass);
	test_enlarge(file, size);
	test_close(file, pass);
	pass++;
    }

    file = test_open(fn, pass);
    buff = test_mmap(file, size);
    if (fill)
	test_fill(buff, size);
    if (sync) {
	printf("Syncing.\n");
	test_sync(file);
    }
    if (verify)
	test_read(buff, size);
    test_munmap(buff, size);
    test_close(file, pass);

    return (EXIT_SUCCESS);
}
EOF
gcc -o test test.c

dd if=/dev/zero of=smallext3 bs=1M count=10
yes|mkfs.ext3 smallext3
mkdir mountpoint

mount -o loop smallext3 mountpoint
# 20M
./test cfs mountpoint/abc 20971520
md5sum mountpoint/abc
./test v mountpoint/abc 20971520
echo Syncing...
sync
./test v mountpoint/abc 20971520
md5sum mountpoint/abc
umount mountpoint
mount -o loop smallext3 mountpoint
./test v mountpoint/abc 20971520
md5sum mountpoint/abc
umount mountpoint
