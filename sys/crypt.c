#define _XOPEN_SOURCE
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char *res;

	if (argc < 2)
		return(EXIT_FAILURE);

	if (argc == 2)
		res = crypt(argv[1], "BS");
	else
		res = crypt(argv[1], argv[2]);

	if (puts(res) < 0) {
		perror("printf");
		return(EXIT_FAILURE);
	}

	return(EXIT_SUCCESS);
}
