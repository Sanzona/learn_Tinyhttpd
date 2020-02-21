#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
void tmp() {
	printf("This is tmp\n");
}

int main() {

	int fd = open("log", O_CREAT|O_WRONLY|O_APPEND, 0644);
	dup2(fd, STDERR_FILENO);
	dup2(fd, STDOUT_FILENO);
	printf("HELLO LIUHAO\n");
	tmp();
	perror("nihao");
//	int a = 1 / 0;
	return 0;
}
