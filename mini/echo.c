/**
 * A simple non-standards-compliant echo implementation for fun
 *
 * TODO:
 * - handle -e HEHE
**/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int arg_pos = 1, opt_pos, drop_opts = 0;
    int print_newline = 1, parse_backslashes = 0;
    char opt;
    if (argc > 1) {
        for (; argv[arg_pos][0] == '-'; arg_pos++) {
            for (opt_pos = 1; (opt = argv[arg_pos][opt_pos]) != '\0';
                 opt_pos++)
                switch (argv[arg_pos][opt_pos]) {
                case 'e':
                    parse_backslashes = 1;
                    break;
                case 'n':
                    print_newline = 0;
                    break;
                case 'E':
                    parse_backslashes = 0;
                    break;
                default:       /* if we hit an unknown option, we print the whole argument */
                    drop_opts = 1;
                    break;
                }
            if (drop_opts)
                break;
        }
        if (!parse_backslashes) {
            for (; arg_pos < argc - 1; arg_pos++) {
                fputs(argv[arg_pos], stdout);
                fputc(' ', stdout);
            }
            fputs(argv[argc - 1], stdout);
        } else {
            /* Implementation left as an exercise :) */
        }
    }
    if (print_newline)
        if (fputc('\n', stdout) == EOF)
            goto err;
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
