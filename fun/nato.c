/**
 * NATO phonetic alphabet one-way converter
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * TODO ponies:
 * - Non-ASCII support
 * - XML-friendly mode
 * - XML mode
 * - XML-RPC API
 * - Fuzzy logic engine
 * - Taking phone calls for you.
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char *LETTERS[] = {
    "<alpha>",
    "<beta>",
    "<charlie>",
    "<delta>",
    "<echo>",
    "<foxtrot>",
    "<golf>",
    "<hotel>",
    "<india>",
    "<juliet>",
    "<kilo>",
    "<lima>",
    "<mike>",
    "<november>",
    "<oscar>",
    "<papa>",
    "<quebec>",
    "<romeo>",
    "<sierra>",
    "<tango>",
    "<uniform>",
    "<victor>",
    "<whisky>",
    "<x-ray>",
    "<yankee>",
    "<zulu>"
};

static inline void printc(char c)
{
    if (c >= 'a' && c < 'z')
        fputs(LETTERS[c - 'a'], stdout);
    else if (c >= 'A' && c < 'Z')
        fputs(LETTERS[c - 'A'], stdout);
    else
        putc(c, stdout);
}

int main(int argc, char **argv)
{
    int ch;
    char *chp;
    if (argc == 1)
        while ((ch = getc(stdin)) != EOF)
            printc((char) ch);
    else if (argc == 2) {
        size_t arglen = strlen(argv[1]);
        for (chp = argv[1]; chp < argv[1] + arglen; chp++)
            printc(*chp);
        putc('\n', stdout);
    } else {
        fprintf(stderr,
                "Usage: %s [message]\n"
                "If no message is given, standard input is used instead.\n",
                argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
