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

#include <sys/types.h>
#include <grp.h>
#include <stdio.h>

void showgrp(const struct group *grp)
{
    if (!printf("%i:%s\n", grp->gr_gid, grp->gr_name))
        exit(2);
}

int main(int argc, char **argv)
{
    const struct group *grp;
    FILE *grpfile;

    if (argc > 1) {
        if (!(grpfile = fopen(argv[1], "r")))
            err(1, "could not open group file '%s'", argv[1]);
        while (grp = fgetgrent(grpfile))
            showgrp(grp);
    } else {
        setgrent();
        while ((grp = getgrent()))
            showgrp(grp);
    }
    return 0;
}
