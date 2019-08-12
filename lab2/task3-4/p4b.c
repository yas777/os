#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv)
{
	if(argc == 2) {
		int fd,n;
		close(0);
		fd = open(argv[1], O_RDWR | O_CREAT);
		pid_t a = fork();
		if (a) {
			sleep(1);
			wait(NULL);

		}
		else {
			printf("Parent: File opened. fd=%d\n",fd);
			char *args[]={"./mycat",NULL};
	        execvp(args[0],args);
	       	// printf("Parent : The child with processedss ID %d has terminated.\n", a);
		}
	}
	else{
		printf("Enter only the filename to be processed\n");
	}
	return 0;
}