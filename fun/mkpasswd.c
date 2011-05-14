/**
 * Simple password generator
 *
 * Copyright (c) 2011, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * By default, the generated password are identically typed on QWERTY,
 * QWERTZ and AZERTY keyboards.
 * They do not include uppercase characters (too annoying on mobile phones).
 *
 * If you think enforcing at least one uppercase letter, two digits and a
 * punctation character makes safer passwords, this tool is not designed
 * for you and I have no interest in your opinion (sorry). 
 *
 * If a "local" brute-force attack is impossible, expect 96 bits to be a
 * "good enough" entropy. Do not quote me on that.
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <err.h>
#include <limits.h>

char *default_chars = "bcdefghijklnoprstuvx"
/* Uppercase is very annoying on many handheld devices.
"BCDEFGHIJKLNOPRSTUVX"
*/
    ;

#define DEFAULT_FILENAME "/dev/random"

int main(int argc, char **argv)
{
    char *filename = DEFAULT_FILENAME;
    char *chars = default_chars;
    FILE *random_file;
    unsigned long int random_long;
    long double entropy_per_char;
    int chars_nr, entropy_required, chars_per_long, chars_required,
        long_pos, read_char, chars_printed;
    unsigned char char_idx;

    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s bits [chars-used [random-file]]\n"
                "bits: number of bits of entropy required\n"
                "chars-used: characters that can be used in the password (%s by default)\n"
                "random-file: randomness source (" DEFAULT_FILENAME
                " by default)\n\n"
                "We are as secure as the randomness source. If your kernel is stupid, tough luck.\n"
                "Multiple occurences of character increase its frequence and breaks entropy computation.\n",
                argv[0], default_chars);
        return (1);
    }

    entropy_required = atoi(argv[1]);
    if (entropy_required < 1)
        errx(2, "at least one bit of entropy is needed");

    if (argc > 2)
        chars = argv[2];

    if (argc > 3)
        filename = argv[3];

    random_file = fopen(filename, "r");
    if (!random_file)
        err(3, "could not open '%s'", filename);

    chars_nr = (int) strlen(chars);
    if (chars_nr < 2 || chars_nr > 255)
        errx(4, "expecting 2 to 256 characters");

    entropy_per_char = log2l((long double) chars_nr);
    chars_required =
        ceill((long double) entropy_required / entropy_per_char);
    chars_per_long =
        floorl((long double) (sizeof(unsigned long int) * CHAR_BIT) /
               entropy_per_char);
    fprintf(stderr,
            "%i bits requested, %i different chars, "
            "%f bits per char, %i chars required, " "%i chars per long\n",
            entropy_required, chars_nr, (double) entropy_per_char,
            chars_required, chars_per_long);

    for (chars_printed = 0; chars_printed < chars_required;
         chars_printed++) {
        if (chars_printed % chars_per_long == 0) {
            /* We need a new random long */
            for (random_long = 0, long_pos = 0;
                 long_pos < (int) sizeof(unsigned long); long_pos++) {
                read_char = fgetc(random_file);
                if (read_char == EOF)
                    goto fd_err;
                random_long <<= sizeof(unsigned char) * CHAR_BIT;
                random_long += (unsigned char) read_char;
            }
        }
        char_idx = (unsigned char) (random_long % chars_nr);
        random_long /= chars_nr;
        putc(chars[char_idx], stdout);
    }

    putc('\n', stdout);
    if (feof(stdout) || fclose(stdout))
        errx(5, "Couldn't finish printing the password");
    return (EXIT_SUCCESS);

  fd_err:
    if (feof(random_file))
        errx(6, "Early EOF in '%s'", filename);
    err(7, "Error reading '%s'", filename);
}
