/**
 * wtfitf: Where's This File In The Filesystem
 *
 * Relies on fiemap.
 * TODO: use bmap when fiemap is not available
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <pierre@gcarrier.fr>
**/

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <errno.h>
#include <string.h>

struct flags_descr {
    const char *name;
    long long unsigned mask;
};

static struct flags_descr known_flags[] = {
    {"FIEMAP_EXTENT_LAST", 0x00000001},
    {"FIEMAP_EXTENT_UNKNOWN", 0x00000002},
    {"FIEMAP_EXTENT_DELALLOC", 0x00000004},
    {"FIEMAP_EXTENT_ENCODED", 0x00000008},
    {"FIEMAP_EXTENT_DATA_ENCRYPTED", 0x00000080},
    {"FIEMAP_EXTENT_NOT_ALIGNED", 0x00000100},
    {"FIEMAP_EXTENT_DATA_INLINE", 0x00000200},
    {"FIEMAP_EXTENT_DATA_TAIL", 0x00000400},
    {"FIEMAP_EXTENT_UNWRITTEN", 0x00000800},
    {"FIEMAP_EXTENT_MERGED", 0x00001000},
    {NULL, 0},
};

void print_flag(const char *name, __u32 flags, int num_flags)
{
    if (num_flags)
        fputs("\t\t\t\t\t\t\t\t\t", stdout);
    if (name)
        fputs(name, stdout);
    else
        printf("unknown 0x%08x", flags);
    fputs("\n", stdout);
}

void print_extent(struct fiemap_extent *extent)
{
    __u32 flags = extent->fe_flags;
    struct flags_descr *cur_descr;
    int num_flags = 0;
    printf("0x%016llx\t0x%016llx\t0x%016llx\t",
           (long long unsigned int) (extent->fe_logical),
           (long long unsigned int) (extent->fe_physical),
           (long long unsigned int) (extent->fe_length));
    if (!flags) {
        printf("none\n");
        return;
    }
    for (cur_descr = known_flags; cur_descr->name; cur_descr++) {
        if (flags & cur_descr->mask) {
            print_flag(cur_descr->name, flags, num_flags);
            flags ^= cur_descr->mask;
            num_flags++;
        }
    }
    if (flags)
        print_flag(NULL, flags, num_flags);
}

int main(int argc, char **argv)
{
    int fd;
    char *filename;
    struct fiemap *map, *old_map;
    unsigned int num_extents, cur_extent;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n"
                "Prints the extents of a file\n", argv[0]);
        goto err;
    }

    filename = argv[1];
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open() failed");
        goto err;
    }

    map = calloc(1, sizeof(struct fiemap));
    if (!map) {
        perror("fiemap allocation failed");
    }
    map->fm_length = FIEMAP_MAX_OFFSET;

    if (ioctl(fd, FS_IOC_FIEMAP, map) < 0) {
        perror("First fiemap failed");
        goto err;
    }

    num_extents = map->fm_mapped_extents + 32;  /* as number can increase between fiemaps... */

    old_map = map;
    map =
        realloc(map,
                sizeof(struct fiemap) +
                num_extents * sizeof(struct fiemap_extent));
    if (!map) {
        free(old_map);
        perror("fiemap reallocation failed");
    }
    map->fm_extent_count = num_extents;

    if (ioctl(fd, FS_IOC_FIEMAP, map) < 0) {
        perror("Final fiemap failed");
        goto err;
    }

    printf("logical offset\t\tphysical offset\t\tlength\t\t\tflags\n");
    for (cur_extent = 0; cur_extent < map->fm_mapped_extents; cur_extent++)
        print_extent(&map->fm_extents[cur_extent]);

    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
