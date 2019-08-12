#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	pid_t a = fork();
	if (a) {
		sleep(1);
		printf("Parent : My process ID is: %d\n", getpid());
		printf("Parent : The child process ID is: %d\n", a);
		wait(NULL);
		printf("Parent : The child with process ID %d has terminated.\n", a);
	}
	else {
		printf("Child : My process ID is: %d\n", getpid());
		printf("Child : The parent process ID is: %d\n", getppid());

	}
	return 0;
}