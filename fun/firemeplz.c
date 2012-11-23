/* Perfect for your production database servers. */

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#define EVER (;;)

enum {
    EXIT_OK = 0,
    EXIT_USAGE,
    EXIT_FAILED
} exit_code;

int main()
{
    char *pages_per_iteration_c, *h;
    intptr_t page_size, alloc_per_iteration, start, current, allocated;
    long pages_per_iteration = 1;
    bool pollute = false;

    start = (intptr_t) sbrk(0);

    page_size = (intptr_t) sysconf(_SC_PAGESIZE);
    fprintf(stderr, "page size: %li\n", (unsigned long) page_size);

    /* Settings */
    if (getenv("POLLUTE") != NULL)
        pollute = true;

    pages_per_iteration_c = getenv("PAGES");
    if (pages_per_iteration_c != NULL) {
        errno = 0;
        pages_per_iteration = strtol(pages_per_iteration_c, NULL, 10);
        if (errno != 0) {
            perror("strtol");
            exit(EXIT_USAGE);
        }
    }

    alloc_per_iteration = pages_per_iteration * page_size;
    allocated = 0;

    for EVER {
        h = (char *) malloc(alloc_per_iteration);
        current = (intptr_t) sbrk(0);
        allocated += pages_per_iteration;

        if (h == NULL) {
            perror("malloc");
            exit(EXIT_FAILED);
        }

        if (pollute == true) {
            intptr_t i;
            for (i = 0; i < pages_per_iteration; i++) {
                *h = 1;
                h += page_size;
            }
        }

        fprintf(stdout, "allocated %li, segment %li..\n",
                allocated, (current - start) / page_size);
    }

    return (EXIT_OK);
}
