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
		exit(1);
	}
	else {
		printf("Child : My process ID is: %d\n", getpid());
		printf("Child : The parent process ID is: %d\n", getppid());
		sleep(5);
		printf("Child : My process ID is: %d\n", getpid());
		printf("Child : The parent process ID is: %d\n", getppid());
		exit(1);
	}
	return 0;
}