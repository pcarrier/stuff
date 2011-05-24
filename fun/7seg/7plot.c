/**
 * A test program for the 7seg library
 *
 * Copyright (c) 2011, Pierre Carrier <code@gcarrier.fr>
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * ./7plot height width message
 *
 * - If width = 0, we will try to print the narrowest text possible.
 * - If compiled with -DRESETSCREEN, we'll clear the screen ourselves.
**/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <err.h>
#include "7seg.h"

static void vert_border(seven_seg_size window) {
    int cur_col;
    for(cur_col = 0; cur_col < window.width + 2; cur_col++) {
        putc('-', stdout);
    }
    putc('\n', stdout);
}

int main (int argc, char const **argv)
{
    seven_seg_size available, used;
    int cur_line, cur_col, length;
    const char *msg;
    char cur_char, **display;

    if(argc != 4)
        goto syntax;
    available.height = atoi(argv[1]);
    available.width = atoi(argv[2]);
    msg = argv[3];

    if(!available.width) {
        length = strlen(msg);
        available.width = 4 * length;
    }

    if(available.height <= 0 || available.width <= 0)
        errx(3, "moron");

    display = malloc(available.height * sizeof(char*));
    if(!display) err(1, "display");

    for(cur_line=0; cur_line < available.height; cur_line++) {
        display[cur_line] = malloc(available.width);
        if(!display[cur_line]) err(1, "display");
    }

    if((errno = seven_seg_plot(display, msg, '#', 'o', available, &used)))
        err(2, "seven_seg_plot");

#ifdef RESETSCREEN
    printf("\033[H\033[2J");
#endif
    printf("%ix%i->%ix%i\n", available.height, available.width, used.height, used.width);
    vert_border(used);
    for(cur_line = 0; cur_line < used.height; cur_line++) {
        putc('|', stdout);
        for(cur_col = 0; cur_col < used.width; cur_col++) {
            cur_char = display[cur_line][cur_col];
            switch(cur_char) {
                case ' ':
                printf("\033[40m \033[0m");
                break;
                case 'o':
                printf("\033[41m \033[0m");
                break;
                case '#':
                printf("\033[47m \033[0m");
                break;
            }
        }
        puts("|");
    }
    vert_border(used);

    return close(STDIN_FILENO);
syntax:
    fprintf(stderr, "Usage; %s height width message\n", argv[0]);
    return EXIT_FAILURE;
}
