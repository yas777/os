#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	pid_t a = fork();
	if (a) {
		// sleep(1);
		// printf("Parent : My Process id %d\n", getpid());
		// printf("The Child process id %d\n", a);
		wait(NULL);
		printf("Parent : The child has terminated.\n");
	}
	else {
		char *args[]={"./mycat",NULL};
        execvp(args[0],args);
	}
	return 0;
}