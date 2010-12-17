/**
 * Superglob! Build a "minimal" glob matching a list of entries
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * TODO:
 * - Find an actual use case?
**/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

int main(int argc, char **argv)
{
    long min_length, arg_length, arg_lengths[argc], arg, pos;
    int nb_chars, current_char, has_trailing_chars =
        0, used_char[CHAR_MAX];
    if (argc <= 2)
        goto err;
    for (arg = 1; arg < argc; arg++) {
        arg_length = strlen(argv[arg]);
        if (min_length > arg_length)
            min_length = arg_length;
        else if (min_length < arg_length)
            has_trailing_chars = 1;
        arg_lengths[arg] = arg_length;
    }
    for (pos = 0; pos < min_length; pos++) {
        memset(used_char, 0, CHAR_MAX * sizeof(int));
        nb_chars = 0;
        for (arg = 1; arg < argc; arg++) {
            current_char = argv[arg][pos] - '\0'; /* positive please :) */
            if (!used_char[current_char]) {
                used_char[current_char] = 1;
                nb_chars++;
            }
        }
        if (nb_chars == 1)
            fputc(current_char, stdout);
        else {
            fputc('[', stdout);
            for (current_char = 0; current_char < CHAR_MAX; current_char++)
                if (used_char[current_char])
                    fputc(current_char, stdout);
            fputc(']', stdout);
        }
    }
    if (has_trailing_chars)
        fputc('*', stdout);
    fputc('\n', stdout);
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
