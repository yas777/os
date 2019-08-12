#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

void child_call(int c){
	if (c>0){
		pid_t a = fork();
		if (a) {
			// sleep(30);
			printf("Child %d is created \n",a);
			wait(NULL);
			printf("Child %d exited \n",a);
		}
		else{
			child_call(--c);
		}
	}

}
int main(int argc, char** argv)
{
	if (argc == 2){
		int n = atoi(argv[1]);
		// printf("%d\n",n);
		pid_t a = fork();
		if (a) {
			printf("Parent is %d\n",getpid());
			
			printf("Number of children is %s\n",argv[1]);
			// int n = argv[1];
			wait(NULL);
			printf("Parent exited\n");
		}
		else child_call(n);
	}
	else {
		printf("Enter only the number of child processes to be created \n");
	}
	return 0;
}