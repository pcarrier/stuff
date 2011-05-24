#ifndef __7SEG_H__
#define __7SEG_H__

/**
 * A text-to-7-segment-like-character-grid rendering engine
 * Written originally for 7clock, a 7-segment clock for ANSI terminals
 *
 * Copyright (c) 2011, Pierre Carrier <code@gcarrier.fr>
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
**/


#include <string.h>
#include <assert.h>
#include <errno.h>

/* Just in case your system is not POSIX enough... */
#ifndef ENOSPC
# define ENOSPC 4242
#endif

typedef struct {
    int height;
    int width;
} seven_seg_size;

/* dest is a table of lines, it must be preallocated, will not be freed.
 * msg is what you want printed there. Unknown characters will be discarded, the list is easy to find at the beginning of 7seg.c.
 * seg_char is the character used for segments, spe_char the one for ':' and '.'.
 * available is the space you allocated in dest,
 * used will contain the space we actually used, it must be preallocated, will not be freed. */
int seven_seg_plot (char **dest, char const *msg, char seg_char, char spe_char, seven_seg_size available, seven_seg_size *used);

#endif
