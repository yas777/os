#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include<fcntl.h>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc == 2) {
		int fd;
		fd = open(argv[1], O_RDWR | O_CREAT);
		pid_t a = fork();
		if (a) {
			// char buf[] = "Hello Worl :Parent";

			sleep(1);
			// printf("Parent : My Process id %d\n", getpid());
			// printf("The Child process id %d\n", a);
			write(fd,"hello world! I am the parent\n", strlen("hello world! I am the parent\n"));
			printf("Parent: File opened. fd=%d\n",fd);
			printf("Parent %d: writing to file %d\n",getpid(),fd);
			wait(NULL);
			printf("Parent : The child has terminated.\n");
			close(fd);
		}
		else {
			// char buf[] = "Hello Worl :Child";
			write(fd, "hello world! I am the child \n", strlen("hello world! I am the child \n"));
			printf("Child %d: writing to file %d\n",getpid(),fd);
		}
	}
	else{
		printf("Enter only the filename to be processed\n");
	}
	return 0;
}