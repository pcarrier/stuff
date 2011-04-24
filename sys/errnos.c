/**
 * errno constants and strings
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Notes:
 * - Not thorough, please report missing E* constants I missed.
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#define UNKNOWN "Unknown error"

void add_errconst(GHashTable *table, int nr, char *str) {
    GList *list = g_hash_table_lookup(table, GINT_TO_POINTER(nr));
    list = g_list_append(list, str); /* g_strdup()ing the string doesn't help */
    g_hash_table_insert(table, GINT_TO_POINTER(nr), list);
}

GHashTable* build_errconsts() {
    GHashTable *res = NULL;
    res = g_hash_table_new(g_direct_hash, g_direct_equal);
    if(res) {
        #undef ERRNO_EXISTS
        #define ERRNO_EXISTS(nr,str) add_errconst(res, nr, str);
        #include "errnos.h"
    }
    return res;
}

int main()
{
    gint errnr, errmax = 256;
    const char *errstr;
    GHashTable *errconsts = build_errconsts();
    GList *cur_errconsts;
    for (errnr = 0; errnr <= errmax; errnr++) {
        errstr = strerror(errnr);
        if(!strncmp(errstr, UNKNOWN, sizeof(UNKNOWN)-1))
            continue;

        printf("%i\t0x%x\t\"%s\"\n", errnr, errnr, errstr);
        cur_errconsts = (GList *) g_hash_table_lookup(errconsts, GINT_TO_POINTER(errnr));
        while(cur_errconsts) {
            printf("\t\t\t%s\n", (char*) cur_errconsts->data);
            cur_errconsts = g_list_next(cur_errconsts);
        }
        errmax = errnr + 256;
    }
    fprintf(stderr, "Stopped looking at %i\n", errnr);
    return fclose(stdout);
}
