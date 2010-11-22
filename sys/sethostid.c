#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> /* required by strerror */

int main(int argc, char **argv) {
	unsigned int length = 0, new_hostid;
	long old_hostid = gethostid();
	if (argc !=2) goto wrong_usage;
	length = strnlen(argv[1], 9);
	if (length != 8) goto wrong_usage;
	sscanf(argv[1], "%x", &new_hostid);
	if((long)new_hostid == old_hostid) {
		fprintf(stderr, "%x is already the hostid.\n", new_hostid);
		return EXIT_SUCCESS;
	}
	fprintf(stderr, "Setting the hostid to %x... ", new_hostid);
	if(sethostid(new_hostid) < 0) {
		fprintf(stderr, "Failed with %u (%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	fprintf(stderr, "Succeeded.\n");
	return EXIT_SUCCESS;
wrong_usage:
	fprintf(stderr, "Usage: %s hostid\nhostid looks like 12cd34ef\n", argv[0]);
	return EXIT_FAILURE;
}
