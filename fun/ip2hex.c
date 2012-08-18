#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	unsigned int a, b, c, d;

	if(argc != 2)
		return EXIT_FAILURE;

	if (sscanf(argv[1], "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
		return EXIT_FAILURE;

	if (printf("%02X%02X%02X%02X\n", a, b, c, d) != 9)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
