#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	pid_t a = fork();
	if (a) {
		printf("Parent : My process ID is: %d\n", getpid());
		printf("Parent : The child process ID is: %d\n", a);
		sleep(60);
		wait(NULL);
		printf("Parent : The child has terminated.\n");
		exit(1);
	}
	else {
		char buf[512];
		printf("Child : My process ID is: %d\n", getpid());
		printf("Child : The parent process ID is: %d\n", getppid());
		int n = read(0, buf, sizeof buf);
		write(1, buf, n);
		exit(1);
	}
	return 0;
}