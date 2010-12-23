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

static const char *NAMES[] = {
    "NULL",                     /* 0x0 */
    "start of heading",         /* 0x1 */
    "start of text",            /* 0x2 */
    "end of Text",              /* 0x3 */
    "end of transmission",      /* 0x4 */
    "enquiry",                  /* 0x5 */
    "acknowledge",              /* 0x6 */
    "bell",                     /* 0x7 */
    "backspace",                /* 0x8 */
    "horizontal tab",           /* 0x9 */
    "new line",                 /* 0xa */
    "vertical tab",             /* 0xb */
    "form feed",                /* 0xc */
    "carriage return",          /* 0xd */
    "shift out",                /* 0xe */
    "shift in",                 /* 0xf */
    "data link escape",         /* 0x10 */
    "device control 1",         /* 0x11 */
    "device control 2",         /* 0x12 */
    "device control 3",         /* 0x13 */
    "device control 4",         /* 0x14 */
    "negative ack",             /* 0x15 */
    "synchronous idle",         /* 0x16 */
    "end of transmission bulk", /* 0x17 */
    "cancel",                   /* 0x18 */
    "end of medium",            /* 0x19 */
    "substitute",               /* 0x1a */
    "escape",                   /* 0x1b */
    "file separator",           /* 0x1c */
    "group separator",          /* 0x1d */
    "record separator",         /* 0x1e */
    "unit separator",           /* 0x1f */
    "space",                    /* 0x20 */
    "exclamation mark",         /* 0x21 */
    "double quote",             /* 0x22 */
    "hash",                     /* 0x23 */
    "dollar",                   /* 0x24 */
    "percent",                  /* 0x25 */
    "ampersand",                /* 0x26 */
    "single quote",             /* 0x27 */
    "opening parenthese",       /* 0x28 */
    "close parenthese",         /* 0x29 */
    "asterisk",                 /* 0x2a */
    "plus",                     /* 0x2b */
    "comma",                    /* 0x2c */
    "minus",                    /* 0x2d */
    "dot",                      /* 0x2e */
    "forward slash",            /* 0x2f */
    "zero",                     /* 0x30 */
    "one",                      /* 0x31 */
    "two",                      /* 0x32 */
    "three",                    /* 0x33 */
    "four",                     /* 0x34 */
    "five",                     /* 0x35 */
    "six",                      /* 0x36 */
    "seven",                    /* 0x37 */
    "height",                   /* 0x38 */
    "nine",                     /* 0x39 */
    "colon",                    /* 0x3a */
    "semicolon",                /* 0x3b */
    "less-than",                /* 0x3c */
    "equal",                    /* 0x3d */
    "bigger-than",              /* 0x3e */
    "question mark",            /* 0x3f */
    "at sign",                  /* 0x40 */
    NULL,                       /* 0x41 */
    NULL,                       /* 0x42 */
    NULL,                       /* 0x43 */
    NULL,                       /* 0x44 */
    NULL,                       /* 0x45 */
    NULL,                       /* 0x46 */
    NULL,                       /* 0x47 */
    NULL,                       /* 0x48 */
    NULL,                       /* 0x49 */
    NULL,                       /* 0x4a */
    NULL,                       /* 0x4b */
    NULL,                       /* 0x4c */
    NULL,                       /* 0x4d */
    NULL,                       /* 0x4e */
    NULL,                       /* 0x4f */
    NULL,                       /* 0x50 */
    NULL,                       /* 0x51 */
    NULL,                       /* 0x52 */
    NULL,                       /* 0x53 */
    NULL,                       /* 0x54 */
    NULL,                       /* 0x55 */
    NULL,                       /* 0x56 */
    NULL,                       /* 0x57 */
    NULL,                       /* 0x58 */
    NULL,                       /* 0x59 */
    NULL,                       /* 0x5a */
    "opening square braket",    /* 0x5b */
    "backslash",                /* 0x5c */
    "closing square bracket",   /* 0x5d */
    "caret",                    /* 0x5e */
    "underscore",               /* 0x5f */
    "reverse single quote",     /* 0x60 */
    "alpha",                    /* 0x61 */
    "bravo",                    /* 0x62 */
    "charlie",                  /* 0x63 */
    "delta",                    /* 0x64 */
    "echo",                     /* 0x65 */
    "foxtrot",                  /* 0x66 */
    "golf",                     /* 0x67 */
    "hotel",                    /* 0x68 */
    "india",                    /* 0x69 */
    "juliet",                   /* 0x6a */
    "kilo",                     /* 0x6b */
    "lima",                     /* 0x6c */
    "mike",                     /* 0x6d */
    "november",                 /* 0x6e */
    "oscar",                    /* 0x6f */
    "papa",                     /* 0x70 */
    "quebec",                   /* 0x71 */
    "romeo",                    /* 0x72 */
    "sierra",                   /* 0x73 */
    "tango",                    /* 0x74 */
    "uniform",                  /* 0x75 */
    "victor",                   /* 0x76 */
    "whiskey",                  /* 0x77 */
    "x-ray",                    /* 0x78 */
    "yankee",                   /* 0x79 */
    "zulu",                     /* 0x7a */
    "opening curly bracket",    /* 0x7b */
    "pipe",                     /* 0x7c */
    "closing curly bracket",    /* 0x7d */
    "tilda",                    /* 0x7e */
    "delete"                    /* 0x7f */
};

static inline void printc(char c)
{
    if (c >= 'A' && c < 'Z') {
        if (printf("uppercase %s, ", NAMES[c - 'A' + 'a']) < 0)
            goto err;
    } else if ((unsigned char) (c - '\0') < 0x80u) {
        if (printf("%s, ", NAMES[c - '\0']) < 0)
            goto err;
    } else if (printf("!dropped!, ") < 0)
        goto err;
    return;
  err:
    exit(EXIT_FAILURE);
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
        if (fputc('\n', stdout) == EOF)
            goto err;
    } else {
        fprintf(stderr,
                "Usage: %s [message]\n"
                "If no message is given, standard input is used instead.\n",
                argv[0]);
        goto err;
    }
    if (fclose(stdout))
        goto err;
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
