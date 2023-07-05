#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>
#include <stdlib.h>

int main()
{
	char buffer[4096];
	int counter = 0;
	setbuf(stdin, NULL);
	while (1) {
		int pid = fork();
		if (pid == 0) {
			int res = scanf("%s", buffer);
			exit(res == EOF);
		} else {
			int status;
			waitpid(pid, &status, 0);
			if (WEXITSTATUS(status) == 1) {
				printf("%d", counter);
				return 0;
			}
			++counter;
		}
	}
}
