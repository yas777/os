#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include<signal.h>
#include <string.h>

int c = 0,c1=0;

void sigchld_handler(int signum)
{
    pid_t pid = wait(NULL);
    c++;
    int   status;
    printf("Parent %d: Child %d exited\n",getpid(),pid);
}

void sigchld_handler1(int signum)
{
    pid_t pid = wait(NULL);
    if (pid > 0) {
    	c1++;
    	int   status;
    	printf("Parent %d: Child %d exited\n",getpid(),pid);
    }
}

int main(int argc, char** argv)
{
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) printf("\ncan't catch SIGINT\n");
	pid_t a = fork();
	pid_t p1 = getpid();
	if (a) {
		while(1);
		printf("Parent exited\n");
	}
	else {
		if (signal(SIGCHLD, sigchld_handler1) == SIG_ERR) printf("\ncan't catch SIGINT\n");
		// int s = 5;
				
		pid_t b = fork();
		pid_t p2 = getpid();
		if(!b) {
			srand(getpid());
			int s = (rand()) % 10;
			sleep(s);
			printf("Child %d is created by parent %d (sleep for %d seconds) \n",getpid(),getppid(),s);
			// sleep(s);
		}else{
			srand(getpid());
			int s = (rand()) % 10;
			printf("Child %d is created by parent %d (sleep for %d seconds) \n",getpid(),getppid(),s);
			sleep(s);
			wait(NULL);
		}

	}
	return 0;
}