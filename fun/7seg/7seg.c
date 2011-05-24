/**
 * A text-to-7-segment-like-character-grid rendering engine
 * Written originally for 7clock, a 7-segment clock for ANSI terminals
 *
 * Copyright (c) 2011, Pierre Carrier <code@gcarrier.fr>
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * Its API is in 7seg.h (what a surprise)
**/

#include "7seg.h"
#include <string.h>

#include <stdio.h>

#define STDCHAR_MIN_WIDTH   3
#define STD2SPE_WIDTH_RATIO 2
#define MIN_HEIGHT          5
#define THICKNESS_DIVIDER_H 10
#define THICKNESS_DIVIDER_W 6
#define CHAR_EMPTY          ' '

/* Seriously, where is this defined in POSIX? */
#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))

typedef enum {
    SEG_TOP      = 1 << 6,
    SEG_TOPLEFT  = 1 << 5,
    SEG_TOPRIGHT = 1 << 4,
    SEG_MIDDLE   = 1 << 3,
    SEG_BOTLEFT  = 1 << 2,
    SEG_BOTRIGHT = 1 << 1,
    SEG_BOTTOM   = 1 << 0,
    SEGS_LEFT    = SEG_TOPLEFT | SEG_BOTLEFT,
    SEGS_RIGHT   = SEG_TOPRIGHT | SEG_BOTRIGHT,
    SEGS_VERT    = SEGS_LEFT | SEGS_RIGHT,
    SEGS_HORIZ   = SEG_TOP | SEG_MIDDLE | SEG_BOTTOM
} segments;

typedef struct {
    char symbol;
    segments repr;
} symbol_repr_t;

static symbol_repr_t symbol_reprs[] = {
    { '0', SEG_TOP | SEG_BOTTOM | SEGS_VERT },
    { '1', SEGS_RIGHT },
    { '2', SEGS_HORIZ | SEG_TOPRIGHT | SEG_BOTLEFT },
    { '3', SEGS_HORIZ | SEGS_RIGHT },
    { '4', SEGS_RIGHT | SEG_TOPLEFT | SEG_MIDDLE },
    { '5', SEGS_HORIZ | SEG_TOPLEFT | SEG_BOTRIGHT },
    { '6', SEGS_HORIZ | SEGS_LEFT | SEG_BOTRIGHT },
    { '7', SEGS_RIGHT | SEG_TOP },
    { '8', SEGS_HORIZ | SEGS_VERT },
    { '9', SEGS_RIGHT | SEGS_HORIZ | SEG_TOPLEFT },
    { 'A', SEG_MIDDLE | SEGS_VERT | SEG_TOP },
    { 'a', SEGS_HORIZ | SEGS_RIGHT | SEG_BOTLEFT },
    { 'B', SEG_MIDDLE | SEG_BOTTOM | SEG_BOTRIGHT | SEGS_LEFT }, /* b to distinguish from 8 */
    { 'b', SEG_MIDDLE | SEG_BOTTOM | SEG_BOTRIGHT | SEGS_LEFT },
    { 'C', SEG_BOTTOM | SEG_TOP | SEGS_LEFT },
    { 'c', SEG_BOTTOM | SEG_MIDDLE | SEG_BOTLEFT },
    { 'D', SEG_MIDDLE | SEG_BOTTOM | SEG_BOTLEFT | SEGS_RIGHT }, /* d to distinguish from 0 */
    { 'd', SEG_MIDDLE | SEG_BOTTOM | SEG_BOTLEFT | SEGS_RIGHT },
    { 'E', SEGS_HORIZ | SEGS_LEFT },
    { 'e', SEGS_HORIZ | SEGS_LEFT | SEG_TOPRIGHT },
    { 'F', SEG_MIDDLE | SEG_TOP | SEGS_LEFT },
    { 'f', SEG_MIDDLE | SEG_TOP | SEGS_LEFT },
    { 'G', SEG_BOTTOM | SEG_TOP | SEGS_LEFT | SEG_BOTRIGHT },
    { 'g', SEG_BOTTOM | SEG_TOP | SEGS_LEFT | SEG_BOTRIGHT }, /* G to distinguish from 9 */
    { 'H', SEGS_RIGHT | SEGS_LEFT | SEG_MIDDLE },
    { 'h', SEG_MIDDLE | SEGS_LEFT | SEG_BOTRIGHT },
    { 'I', SEGS_LEFT },
    { 'i', SEG_BOTLEFT },
    { 'J', SEG_BOTTOM | SEGS_RIGHT },
    { 'j', SEG_BOTTOM | SEG_BOTRIGHT | SEG_TOP },
    { 'K', SEGS_RIGHT | SEGS_LEFT | SEG_MIDDLE }, /* like H */
    { 'k', SEG_MIDDLE | SEGS_LEFT | SEG_BOTRIGHT }, /* like h */
    { 'L', SEG_BOTTOM | SEGS_LEFT },
    { 'l', SEG_BOTTOM | SEGS_LEFT }, /* L to distinguish from I/1 */
    { 'M', SEGS_RIGHT | SEGS_LEFT | SEG_TOP },
    { 'm', SEG_MIDDLE | SEG_BOTLEFT | SEG_BOTRIGHT },
    { 'N', SEGS_RIGHT | SEGS_LEFT | SEG_TOP }, /* like M */
    { 'n', SEG_MIDDLE | SEG_BOTLEFT | SEG_BOTRIGHT }, /* like m */
    { 'O', SEG_MIDDLE | SEG_BOTLEFT | SEG_BOTRIGHT | SEG_BOTTOM }, /* o to distinguish from 0 */
    { 'o', SEG_MIDDLE | SEG_BOTLEFT | SEG_BOTRIGHT | SEG_BOTTOM },
    { 'P', SEG_MIDDLE | SEG_TOP | SEG_TOPRIGHT | SEGS_LEFT },
    { 'p', SEG_MIDDLE | SEG_TOP | SEG_TOPRIGHT | SEGS_LEFT }, /* like P */
    { 'Q', SEG_MIDDLE | SEG_TOP | SEG_TOPLEFT | SEGS_RIGHT }, /* like q */
    { 'q', SEG_MIDDLE | SEG_TOP | SEG_TOPLEFT | SEGS_RIGHT },
    { 'R', SEG_MIDDLE | SEG_BOTLEFT }, /* like r */
    { 'r', SEG_MIDDLE | SEG_BOTLEFT },
    { 'S', SEGS_HORIZ | SEG_TOPLEFT | SEG_BOTRIGHT }, /* like 5 */
    { 's', SEGS_HORIZ | SEG_TOPLEFT | SEG_BOTRIGHT }, /* like 5 */
    { 'T', SEG_MIDDLE | SEGS_LEFT | SEG_BOTTOM }, /* like t */
    { 't', SEG_MIDDLE | SEGS_LEFT | SEG_BOTTOM },
    { 'U', SEGS_RIGHT | SEGS_LEFT | SEG_BOTTOM },
    { 'u', SEG_BOTTOM | SEG_BOTRIGHT | SEG_BOTLEFT },
    { 'V', SEGS_RIGHT | SEGS_LEFT | SEG_BOTTOM }, /* like U */
    { 'v', SEG_BOTTOM | SEG_BOTRIGHT | SEG_BOTLEFT }, /* like u */
    { 'W', SEGS_RIGHT | SEGS_LEFT | SEG_BOTTOM }, /* like U */
    { 'w', SEG_BOTTOM | SEG_BOTRIGHT | SEG_BOTLEFT }, /* like u */    
    { 'X', SEGS_RIGHT | SEGS_LEFT | SEG_MIDDLE }, /* like H */
    { 'x', SEGS_RIGHT | SEGS_LEFT | SEG_MIDDLE }, /* like H */
    { 'Y', SEG_MIDDLE | SEG_BOTTOM | SEGS_RIGHT | SEG_TOPLEFT }, /* like y */
    { 'y', SEG_MIDDLE | SEG_BOTTOM | SEGS_RIGHT | SEG_TOPLEFT },
    { 'Z', SEGS_HORIZ | SEG_TOPRIGHT | SEG_BOTLEFT }, /* like 2 */
    { 'z', SEGS_HORIZ | SEG_TOPRIGHT | SEG_BOTLEFT }, /* like 2 */
    { '_', SEG_BOTTOM },
    { '-', SEG_MIDDLE },
    { ' ', 0 },
    { '\0', 0 } /* EOL */
};

static symbol_repr_t * find_repr(char c)
{
    symbol_repr_t * res = symbol_reprs;
    while(res->symbol != '\0' && res->symbol != c)
        res++;
    if(res->symbol != '\0')
        return res;
    return NULL;
}

static void count_chars(char const *msg, int msg_len, int * nb_stdchar, int * nb_spechar)
{
    int pos = 0;
    char c;
    *nb_stdchar = 0;
    *nb_spechar = 0;
    for(pos = 0; pos < msg_len; pos++) {
        c = msg[pos];
        if(c == ':' || c == '.' || c == ';')
            (*nb_spechar)++;
        if(find_repr(c))
            (*nb_stdchar)++;
    }
}

static void draw_rect(char **dest, int x1, int x2, int y1, int y2, char filler) {
    int x, y;
    for(y = y1; y <= y2; y++)
        for(x = x1; x <= x2; x++)
            dest[y][x] = filler;
}

static void draw_repr(char ** dest, int segment, unsigned int pos, unsigned int height, unsigned int width, char filler)
{
    int thickness = MIN(height / THICKNESS_DIVIDER_H, width / THICKNESS_DIVIDER_W);
    int middle_line = (height-1) / 2;

        #define BORDER_LEFT (pos)
        #define BORDER_RIGHT (pos+width-1)
        #define BORDER_TOP (0)
        #define BORDER_BOTTOM (height-1)
        if(segment & SEG_TOP)
            draw_rect(dest, BORDER_LEFT, BORDER_RIGHT, BORDER_TOP, BORDER_TOP + thickness, filler);
        if(segment & SEG_MIDDLE)
            draw_rect(dest, BORDER_LEFT, BORDER_RIGHT,
                middle_line - thickness + thickness/2, middle_line + thickness/2, filler);
        if(segment & SEG_BOTTOM)
            draw_rect(dest, BORDER_LEFT, BORDER_RIGHT, BORDER_BOTTOM - thickness, BORDER_BOTTOM, filler);
        if(segment & SEG_TOPLEFT)
            draw_rect(dest, BORDER_LEFT, BORDER_LEFT+thickness, BORDER_TOP, middle_line, filler);
        if(segment & SEG_BOTLEFT)
            draw_rect(dest, BORDER_LEFT, BORDER_LEFT+thickness, middle_line, BORDER_BOTTOM, filler);
        if(segment & SEG_TOPRIGHT)
            draw_rect(dest, BORDER_RIGHT-thickness, BORDER_RIGHT, BORDER_TOP, middle_line, filler);
        if(segment & SEG_BOTRIGHT)
            draw_rect(dest, BORDER_RIGHT-thickness, BORDER_RIGHT, middle_line, BORDER_BOTTOM, filler);
        #undef BORDER_LEFT
        #undef BORDER_RIGHT
        #undef BORDER_TOP
        #undef BORDER_BOTTOM
}

int seven_seg_plot (char **dest, char const *msg, char seg_char, char spe_char, seven_seg_size available, seven_seg_size *used)
{
    int msg_len;
    int nb_stdchar, nb_spechar, nb_chars, nb_spaces;
    int stdchar_width, spechar_width, space_width, real_width;
    int line, colon_line1, colon_line2, spechar_shift;
    int dest_pos = 0;
    char * current_char;
    symbol_repr_t * current_repr;

    msg_len = strlen(msg);

    count_chars(msg, msg_len, &nb_stdchar, &nb_spechar);
    nb_chars = nb_stdchar + nb_spechar;
    nb_spaces = nb_chars - 1;

    if ((available.height < MIN_HEIGHT) ||
        (available.width < STDCHAR_MIN_WIDTH * nb_stdchar + nb_spechar + nb_spaces))
        return ENOSPC;

    if(nb_spechar || nb_stdchar) {
        stdchar_width = (STD2SPE_WIDTH_RATIO * (available.width - nb_spaces)) / (nb_spechar + nb_stdchar * STD2SPE_WIDTH_RATIO);
        spechar_width = stdchar_width / STD2SPE_WIDTH_RATIO;
    }
    if(!spechar_width)
        spechar_width++;

    if(nb_spaces)
        space_width = (available.width - nb_stdchar * stdchar_width - nb_spechar * spechar_width) / nb_spaces;
    if(!space_width)
        space_width = 1;

    real_width = nb_stdchar * stdchar_width + nb_spechar * spechar_width + nb_spaces * space_width;

    /* Clear the zone */
    for(line = 0; line < available.height; line++)
        memset((void*)(dest[line]), CHAR_EMPTY, real_width);
    
    spechar_shift = (spechar_width > 1) ? spechar_width/2 - 1 : 0;
    colon_line1 = (available.height-1)*2/5;
    colon_line2 = (available.height-1)*4/5;
    if(colon_line1 >= available.height/2)
        colon_line1--;
    if(colon_line2 - colon_line1 < 2)
        colon_line2++;

    for(current_char = (char *)msg; current_char < msg + msg_len; current_char++) {
        switch(*current_char) {
        case ':':
            dest[colon_line1][dest_pos+spechar_shift] = spe_char;
            dest[colon_line2][dest_pos+spechar_shift] = spe_char;
            dest_pos += (space_width + spechar_width);
            break;
        case '.':
            dest[available.height-1][dest_pos+spechar_shift] = spe_char;
            dest_pos += (space_width + spechar_width);
            break;
        /* Takes the same amount of space as a : (used for blinking clocks) */
        case ';':
            dest_pos += (space_width + spechar_width);
            break;
        default:
            if((current_repr = find_repr(*current_char))) {
                draw_repr(dest, current_repr->repr, dest_pos, available.height, stdchar_width, seg_char);
                dest_pos += (space_width + stdchar_width);
            }
            break;
        }
    }

    if(used) {
        used->height = available.height;
        used->width = real_width;
    }

    return 0;
}
