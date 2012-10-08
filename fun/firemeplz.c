/* Perfect for your production database servers. */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define EVER (;;)

enum {
	EXIT_OK = 0,
	EXIT_USAGE,
	EXIT_FAILED,
} exit_code;

int main(int argc, char **argv) {
	long incr;
	void *h;
	const char DOT = '.';

	if (argc !=2)
		exit(EXIT_USAGE);

	errno = 0;
	incr = strtol(argv[1], NULL, 10);
	if (errno != 0) {
		perror("strtol");
		exit(EXIT_USAGE);
	}

	for EVER {
		h = malloc(incr);

		if (!h) {
			perror("malloc");
			exit(EXIT_FAILED);
		}

		write(STDOUT_FILENO, &DOT, 1);
	}

	return (EXIT_OK);
}
