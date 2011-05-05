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
 * gcc -shared -fPIC -ldl -o /usr/local/lib/gdb4undeads.so gdb4undeads.c
 * Use with:
 * LD_PRELOAD=/usr/local/lib/gdb4undeads.so gcore [...]
 * LD_PRELOAD=/usr/local/lib/gdb4undeads.so gdb [...]
 *
 * Warning: this has the trivial side-effect of messing with symlink values starting
 * or finishing with '(deleted)' in /proc (sooo likely). If you have some of those,
 * BEWARE!
**/

#ifdef __APPLE__
# define _DARWIN_C_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <dlfcn.h>

typedef ssize_t readlink_t (const char *, char *, size_t);

ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
    char ibuf[PATH_MAX];
    char *result = ibuf;
    ssize_t rc;
    readlink_t * orig_readlink = (readlink_t *) dlsym(RTLD_NEXT, "readlink");
    rc = orig_readlink(path, ibuf, bufsiz);
    if (rc < 0)
        goto done;
    if (!strncmp(path, "/proc", 5)) {
        if (!strncmp(ibuf, "(deleted) ", 10)) {
            rc -= 10;
            result += 10;
            fprintf(stderr, "[gdb4undeads: '%s' was prefixed]\n", ibuf);
        } else if (!strncmp(ibuf + strlen(ibuf) - 10, " (deleted)", 10)) {
            rc -= 10;
            fprintf(stderr, "[gdb4undeads: '%s' was postfixed]\n", ibuf);
        }
    }
    strncpy(buf, result, rc);
  done:
    return rc;
}
