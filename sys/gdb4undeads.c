/**
 * gdb4undeads: fix gcore/gdb for processes where the binary was removed and
 * is available again.
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Example use case: nash process from initrd, after the rootfs pivot
 * (requires ln -sf /sbin/nash /bin/nash under RHEL5)
 *
 * Compile with:
 * gcc -g -shared -fPIC -ldl -o /usr/local/lib/gdb4undeads.so gdb4undeads.c
 * Use with:
 * LD_PRELOAD=/usr/local/lib/gdb4undeads.so gcore [...]
 * LD_PRELOAD=/usr/local/lib/gdb4undeads.so gdb [...]
**/

#define _GNU_SOURCE 1
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <dlfcn.h>

ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
    char tmp_path[PATH_MAX];
    char *result = tmp_path;
    ssize_t rc;
    ssize_t(*orig_readlink) (const char *path, char *buf, size_t bufsiz) =
	dlsym(RTLD_NEXT, "readlink");
    rc = orig_readlink(path, tmp_path, bufsiz);
    if (rc < 0)
	goto done;
    if (!strncmp(tmp_path, "(deleted) ", 10)) {
	rc -= 10;
	result += 10;
	fprintf(stderr, "[gdb4undeads: '%s' was prefixed]\n", tmp_path);
    } else
	if (!strncmp(tmp_path + strlen(tmp_path) - 10, " (deleted)", 10)) {
	rc -= 10;
	fprintf(stderr, "[gdb4undeads: '%s' was postfixed]\n", tmp_path);
    }
    strncpy(buf, result, rc);
  done:
    return rc;
}

