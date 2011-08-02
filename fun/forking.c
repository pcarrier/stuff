#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void main() {
	int loop = 0;
	pid_t p;
	while (loop < 2048) {
		p = fork();
		if (p) { /* in parent */
			waitpid(p, NULL, 0);
			exit(0);
		}
		else /* in child */
			loop++;
	}
}
