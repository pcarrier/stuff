/**
 * List groups
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Usage: list-groups [/etc/group]
 * If no file is specified, goes through all nss backends
 */

#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>
#include <sys/types.h>
#include <grp.h>
#include <err.h>

void showgrp(const struct group *grp)
{
    char **gr_mem;
    if (!printf("%s:%s:%i:", grp->gr_name, grp->gr_passwd, grp->gr_gid))
        exit(EX_IOERR);
    for (gr_mem = grp->gr_mem; **gr_mem != '\0'; gr_mem++)
        if (!printf("%s,", *gr_mem))
            exit(EX_IOERR);
    if (!puts(""))
        exit(EX_IOERR);
}

int main(int argc, char **argv)
{
    const struct group *grp;
#ifdef HAVE_FGETGRENT
    FILE *grpfile;
#endif

    if (argc > 2)
        goto usage;

    if (argc == 2) {
#ifdef HAVE_FGETGRENT
        if (!(grpfile = fopen(argv[1], "r")))
            err(EX_OSFILE, "could not open group file '%s'", argv[1]);
        while ((grp = fgetgrent(grpfile)))
            showgrp(grp);
#else
        goto usage;
#endif
    } else {
        setgrent();
        while ((grp = getgrent()))
            showgrp(grp);
    }
    return EX_OK;

  usage:
    errx(EX_USAGE, "Usage: %s [group-file]", argv[0]);
}
