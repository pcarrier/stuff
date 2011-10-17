#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	int loop = 0;
	pid_t p;
	while (loop < 2048) {
		p = fork();
		if (p < 0) {
			fprintf(stderr, "Failed at loop = %i!\n", loop);
			return(EXIT_FAILURE);
		}
		if (p) { /* in parent */
			waitpid(p, NULL, 0);
			return(EXIT_SUCCESS);
		}
		else /* in child */
			loop++;
	}
	return(EXIT_FAILURE); /* impossible */
}
