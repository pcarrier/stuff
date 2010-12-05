/**
 * A simple non-standards-compliant echo implementation for fun
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static inline void print(char *str, int parse_backslashes)
{
    char *remaining = str, *cur_pos = str, *end = str + strlen(str);
    int backslashed = ' ', attempted_backslashed;
    if (parse_backslashes) {
        while (remaining < end) {
            cur_pos = strchr(remaining, '\\');
            if (cur_pos == NULL) {
                fputs(remaining, stdout);
                break;
            }
            fwrite(remaining, sizeof(char), cur_pos - remaining, stdout);
#define ESCAPED_REGISTER(c, p) case c: fputc(p, stdout); ++cur_pos; break;
            switch (*(++cur_pos)) {
                ESCAPED_REGISTER('\\', '\\');
                ESCAPED_REGISTER('a', '\a');
                ESCAPED_REGISTER('b', '\b');
                ESCAPED_REGISTER('e', '\e');
                ESCAPED_REGISTER('f', '\f');
                ESCAPED_REGISTER('n', '\n');
                ESCAPED_REGISTER('r', '\r');
                ESCAPED_REGISTER('t', '\t');
                ESCAPED_REGISTER('v', '\v');
            case '0':
                attempted_backslashed = 0;
                for (cur_pos++;
                     *cur_pos >= '0' && *cur_pos <= '8'; cur_pos++) {
                    attempted_backslashed *= 8;
                    attempted_backslashed += (*cur_pos - '0');
                    if (attempted_backslashed <= 0xff)
                        backslashed = attempted_backslashed;
                    else
                        break;
                }
                fputc(backslashed, stdout);
                break;
            case 'x':
                attempted_backslashed = 0;
                for (cur_pos++;
                     ((*cur_pos >= '0' && *cur_pos <= '9') ||
                      (*cur_pos >= 'a' && *cur_pos <= 'f') ||
                      (*cur_pos >= 'A' && *cur_pos <= 'F')); cur_pos++) {
                    attempted_backslashed *= 16;
                    if (*cur_pos >= 'a')
                        attempted_backslashed += 10 + (*cur_pos - 'a');
                    else if (*cur_pos >= 'A')
                        attempted_backslashed += 10 + (*cur_pos - 'A');
                    else
                        attempted_backslashed += (*cur_pos - '0');
                    if (attempted_backslashed <= 0xff)
                        backslashed = attempted_backslashed;
                    else
                        break;
                }
                fputc(backslashed, stdout);
                break;
            }
            remaining = cur_pos;
        }

    } else {
        fputs(str, stdout);
    }
}

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
                case '-':      /* implements '--' from 1003.1, & more */
                    drop_opts = 1;
                    opt_pos++;
                    break;
                default:       /* print the whole argument if unknown opt */
                    drop_opts = 1;
                    break;
                }
            if (drop_opts)
                break;
        }
        for (; arg_pos < argc - 1; arg_pos++) {
            print(argv[arg_pos], parse_backslashes);
            fputc(' ', stdout);
        }
        print(argv[argc - 1], parse_backslashes);
    }
    if (print_newline)
        if (fputc('\n', stdout) == EOF)
            goto err;
    if (fclose(stdout))
        goto err;
    return EXIT_SUCCESS;
  err:
    return EXIT_FAILURE;
}
