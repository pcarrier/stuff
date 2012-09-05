#include <errno.h>
#include <sys/timex.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

int main(int argc, char **argv)
{
    struct timex t;
    int rc;

    if (argc < 2)
        exit(EXIT_FAILURE);

    bzero(&t, sizeof(struct timex));

    if (adjtimex(&t) < 0) {
        perror("adjtimex read");
        return (EXIT_FAILURE);
    }

    t.modes = ADJ_STATUS;
    switch (argv[1][0]) {
    case '=':
        t.status &= ~STA_INS;
        t.status &= ~STA_DEL;
        break;
    case '+':
        t.status |= STA_INS;
        t.status &= ~STA_DEL;
        break;
    case '-':
        t.status &= ~STA_INS;
        t.status |= STA_DEL;
        break;
    default:
        fprintf(stderr, "unknown operation %c\n", argv[1][0]);
        return (EXIT_FAILURE);
    }

    rc = adjtimex(&t);
    if (rc < 0) {
        perror("adjtimex write");
        return (EXIT_FAILURE);
    }

    printf("%i:%i\n", t.status, rc);
    return (EXIT_SUCCESS);
}
