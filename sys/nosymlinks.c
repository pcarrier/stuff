/**
 * nosymlinks: FUSE FS dereferencing symlinks
 *
 * Copyright (c) 2010, Red Hat Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
**/

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static int nsl_getattr(const char *path, struct stat *stbuf)
{
    if (lstat(path, stbuf) < 0)
        return -errno;
    return 0;
}

static int nsl_access(const char *path, int mask)
{
    if (access(path, mask) < 0)
        return -errno;
    return 0;
}

static int nsl_readlink(const char *path, char *buf, size_t size)
{
    int read;
    read = readlink(path, buf, size - 1);
    if (res < 0)
        return -errno;
    buf[res] = '\0';
    return 0;
}


static int
nsl_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
            off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int nsl_mknod(const char *path, mode_t mode, dev_t rdev)
{
    if (mknod(path, mode, rdev) < 0)
        return -errno;
    return 0;
}

static int nsl_mkdir(const char *path, mode_t mode)
{
    if (mkdir(path, mode) < 0)
        return -errno;
    return 0;
}

static int nsl_unlink(const char *path)
{
    if (unlink(path) < 0)
        return -errno;
    return 0;
}

static int nsl_rmdir(const char *path)
{
    if (rmdir(path) < 0)
        return -errno;
    return 0;
}

static int nsl_symlink(const char *src, const char *dst)
{
    if (symlink(src, dst) < 0)
        return -errno;
    return 0;
}

static int nsl_rename(const char *src, const char *dst)
{
    if (rename(from, to) < 0)
        return -errno;
    return 0;
}

static int nsl_link(const char *src, const char *dst)
{
    if (link(src, dst) < 0)
        return -errno;
    return 0;
}

static int nsl_chmod(const char *path, mode_t mode)
{
    if (chmod(path, mode) < 0)
        return -errno;
    return 0;
}

static int nsl_chown(const char *path, uid_t uid, gid_t gid)
{
    if (lchown(path, uid, gid) < 0)
        return -errno;
    return 0;
}

static int nsl_truncate(const char *path, off_t size)
{
    if (truncate(path, size) < 0)
        return -errno;
    return 0;
}

static int nsl_utimens(const char *path, const struct timespec ts[2])
{
    int res;
    struct timeval tv[2];

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(path, tv);
    if (res == -1)
        return -errno;

    return 0;
}

static int nsl_open(const char *path, struct fuse_file_info *fi)
{
    int fd;
    fd = open(path, fi->flags);
    if (fd < 0)
        return -errno;
    close(fd);
    return 0;
}

static int
nsl_read(const char *path, char *buf, size_t size, off_t offset,
         struct fuse_file_info *fi)
{
    int fd;
    int ret = 0;
    fd = open(path, O_RDONLY);
    if (fd < 0)
        return -errno;
    if (pread(fd, buf, size, offset) < 0)
        err = -errno;
    close(fd);
    return ret;
}

static int
nsl_write(const char *path, const char *buf, size_t size,
          off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;

    (void) fi;
    fd = open(path, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int nsl_statfs(const char *path, struct statvfs *stbuf)
{
    int res;

    res = statvfs(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int nsl_release(const char *path, struct fuse_file_info *fi)
{
    /* Just a stub.  This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) fi;
    return 0;
}

static int
nsl_fsync(const char *path, int isdatasync, struct fuse_file_info *fi)
{
    /* Just a stub.  This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) isdatasync;
    (void) fi;
    return 0;
}

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int
nsl_setxattr(const char *path, const char *name,
             const char *value, size_t size, int flags)
{
    int res = lsetxattr(path, name, value, size, flags);
    if (res == -1)
        return -errno;
    return 0;
}

static int
nsl_getxattr(const char *path, const char *name, char *value, size_t size)
{
    int res = lgetxattr(path, name, value, size);
    if (res == -1)
        return -errno;
    return res;
}

static int nsl_listxattr(const char *path, char *list, size_t size)
{
    int res = llistxattr(path, list, size);
    if (res == -1)
        return -errno;
    return res;
}

static int nsl_removexattr(const char *path, const char *name)
{
    int res = lremovexattr(path, name);
    if (res == -1)
        return -errno;
    return 0;
}
#endif                          /* HAVE_SETXATTR */

static struct fuse_operations nsl_ops = {
    .getattr = nsl_getattr,
    .access = nsl_access,
    .readlink = nsl_readlink,
    .readdir = nsl_readdir,
    .mknod = nsl_mknod,
    .mkdir = nsl_mkdir,
    .symlink = nsl_symlink,
    .unlink = nsl_unlink,
    .rmdir = nsl_rmdir,
    .rename = nsl_rename,
    .link = nsl_link,
    .chmod = nsl_chmod,
    .chown = nsl_chown,
    .truncate = nsl_truncate,
    .utimens = nsl_utimens,
    .open = nsl_open,
    .read = nsl_read,
    .write = nsl_write,
    .statfs = nsl_statfs,
    .release = nsl_release,
    .fsync = nsl_fsync,
#ifdef HAVE_SETXATTR
    .setxattr = nsl_setxattr,
    .getxattr = nsl_getxattr,
    .listxattr = nsl_listxattr,
    .removexattr = nsl_removexattr,
#endif
};

int main(int argc, char **argv)
{
    return fuse_main(argc, argv, &nsl_ops);
}
