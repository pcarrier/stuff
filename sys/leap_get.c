#include <errno.h>
#include <sys/timex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    struct timex t;
    int rc;

    memset(&t, 0, sizeof(struct timex));

    rc = adjtimex(&t);

    switch (rc) {
    case -1:
        perror("adjtimex");
        return (EXIT_FAILURE);
    case TIME_OK:
        puts("synced");
        break;
    case TIME_INS:
        puts("insert");
        break;
    case TIME_DEL:
        puts("delete");
        break;
    case TIME_OOP:
        puts("in progress");
        break;
    case TIME_WAIT:
        puts("has occured");
        break;
    case TIME_BAD:
        puts("not synced");
        break;
    default:
        fprintf(stderr, "gnnn %i\n", rc);
        return (EXIT_FAILURE);
    }

    if (getenv("DEBUG")) {
        printf("%#x\n", t.status);
    }

    return (EXIT_SUCCESS);
}
