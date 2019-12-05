/**
 * A full-screen 7-segment clock for ANSI terminals
 *
 * Copyright (c) 2011, Pierre Carrier <code@gcarrier.fr>
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
**/

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "7seg.h"

int self_pipe[2];


/* I need addressable char constants for event types.
 * This is the best approach I could reach */

typedef char event_t;

enum {
    WINDOW_WAS_RESIZED = 0x42,
    NEW_TIME_TO_DISPLAY = 0x43,
    WANTS_TO_LEAVE = 0x44
};

struct {
    event_t window_was_resized;
    event_t new_time_to_display;
    event_t wants_to_leave;
} events = {
WINDOW_WAS_RESIZED, NEW_TIME_TO_DISPLAY, WANTS_TO_LEAVE};


static void notify_event(event_t event)
{
    write(self_pipe[1], &event, 1);
}

static void new_window_size(int sig)
{
    notify_event(events.window_was_resized);
    (void)sig;
}

static void new_time(int sig)
{
    notify_event(events.new_time_to_display);
    (void)sig;
}

static void leaving(int sig)
{
    notify_event(events.wants_to_leave);
    (void)sig;
}

static void set_non_blocking(int fd)
{
    int fd_flags;
    if ((fd_flags = fcntl(fd, F_GETFL)) < 0)
        err(1, "F_GETFL %i", fd);
    if (fcntl(fd, F_SETFD, fd_flags | O_NONBLOCK) < 0)
        err(1, "F_SETFD %i", fd);
}

static void update_window(seven_seg_size * window, char ***clock_text_ptr)
{
    int cur_line;
    struct winsize term_win;

    /* Free the currently used space */
    if (*clock_text_ptr) {
        for (cur_line = 0; cur_line < window->height; cur_line++) {
            free((*clock_text_ptr)[cur_line]);
        }
        free(*clock_text_ptr);
    }

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_win))
        err(7, "getting the window size");
    window->height = term_win.ws_row;
    window->width = term_win.ws_col;

    /* Allocate a new one */
    *clock_text_ptr = malloc(window->height * sizeof(char *));
    if (!*clock_text_ptr)
        goto alloc_fail;
    for (cur_line = 0; cur_line < window->height; cur_line++) {
        (*clock_text_ptr)[cur_line] = malloc(window->width);
        if (!(*clock_text_ptr)[cur_line])
            goto alloc_fail;
    }
    /* clear screen */
    printf("\033[2J");
    return;
  alloc_fail:
    err(8, "alloc window");
}

static void display_time(seven_seg_size * window, char **clock_text)
{
    time_t raw_time;
    struct tm *timeinfo;
    char timestr[LINE_MAX], cur_char;
    seven_seg_size used;
    int cur_col, cur_line, timestrlen;

    time(&raw_time);
    timeinfo = localtime(&raw_time);
    strftime(timestr, LINE_MAX, "%H:%M:%S", timeinfo);
    timestrlen = strlen(timestr);

    /* blinking last : */
    if (timestr[timestrlen - 1] % 2 == 1)
        timestr[timestrlen - 3] = ';';

    if ((errno =
         seven_seg_plot(clock_text, timestr, '#', 'o', *window, &used)))
        err(43, "plotting");

    /* back to top-left corner */
    printf("\033[H");
    for (cur_line = 0; cur_line < used.height; cur_line++) {
        for (cur_col = 0; cur_col < used.width; cur_col++) {
            cur_char = clock_text[cur_line][cur_col];
            switch (cur_char) {
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
        /* no need for a newline if we have exactly the term size */
        if (used.width < window->width && cur_line < used.height - 1) {
            putc('\n', stdout);
        }
    }
    fflush(stdout);
}

int main()
{
    struct itimerval every_sec;
    struct sigaction sa;
    struct pollfd polled_fd;
    char event;

    seven_seg_size window;
    char **clock_text = NULL;

    if (!isatty(STDOUT_FILENO)) {
        errx(42, "not a terminal");
    }

    /* Pipe with non-blocking write */
    if (pipe(self_pipe))
        err(1, "self-pipe creation");
    set_non_blocking(self_pipe[1]);

    /* signal handlers in place */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_handler = new_window_size;
    if (sigaction(SIGWINCH, &sa, NULL) < 0)
        err(5, "handling SIGWINCH");
    sa.sa_handler = new_time;
    if (sigaction(SIGALRM, &sa, NULL) < 0)
        err(4, "handling SIGARLM");
    sa.sa_handler = leaving;
    if (sigaction(SIGINT, &sa, NULL) < 0)
        err(6, "handling SIGINT");

    /* only poll on the reading end of self_pipe */
    polled_fd.fd = self_pipe[0];
    polled_fd.events = POLLIN;

    /* throw SIGALRMs every second */
    memset((void *) &every_sec, 0, sizeof(struct itimerval));
    every_sec.it_value.tv_sec = 1;
    every_sec.it_interval.tv_sec = 1;
    setitimer(ITIMER_REAL, &every_sec, NULL);

    /* initial window display */
    printf("\033[?25l");        /* hide cursor */
    update_window(&window, &clock_text);
    display_time(&window, clock_text);

    for (;;) {
        if (poll(&polled_fd, 1, -1 /* block forever */ ) < 0
            && errno != EINTR)
            err(2, "polling");

        else {
            if (read(self_pipe[0], &event, 1) < 1)
                err(3, "read %i", self_pipe[0]);

            switch (event) {
            case WINDOW_WAS_RESIZED:
                update_window(&window, &clock_text);
                display_time(&window, clock_text);
                break;
            case NEW_TIME_TO_DISPLAY:
                display_time(&window, clock_text);
                break;
            case WANTS_TO_LEAVE:
                printf("\033[?25h");    /* show cursor */
                exit(0);
            }
        }
    }
}
