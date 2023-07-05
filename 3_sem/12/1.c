#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
int main() {
	char buff[9024];
	int input = open("temp.c", O_CREAT | O_TRUNC | O_RDWR, 0644);
	char code[] = "#include <stdio.h>\nint main() {\nprintf( \"%d\", (";
	write(input, code, sizeof(code) - 1);
	int size = read(0, &buff, sizeof(buff));
	write(input, buff, size);
	char end[] = "));\nreturn 0;}";
	write(input, end, sizeof(end) - 1); 
	if (fork() == 0) {
		execlp("gcc", "gcc", "temp.c", NULL);
	} else {
		wait(NULL);
		execlp("./a.out", "./a.out", NULL);
	}
}
