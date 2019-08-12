#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include<signal.h>
#include <string.h>

int c = 0;

void sigchld_handler(int signum)
{
    pid_t pid = wait(NULL);
    c++;
    int   status;
    printf("Parent %d: Child Terminated %d\n",getpid(),pid);
    // while ((pid = waitpid(-1, &status, WNOHANG)) != -1)
    // {
    //    if(pid <= 0) break;
    //    else printf("Child Terminated %d\n",pid);   // Or whatever you need to do with the PID
    // }
}


int main(int argc, char** argv)
{
	if (argc == 2){
	
		int status=0, n = atoi(argv[1]);
		pid_t a[n],wpid;
		pid_t  ppid = getpid();;
	  	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) printf("\ncan't catch SIGINT\n");
		for (int i=0;i<n;i++){
			if (i<n) {
				a[i] = fork();
				if (a[i] && getpid() == ppid){
					
					// wait(NULL);
					continue;
				}
				else{
					srand(getpid());
					int s = (rand()) % 10;
					// sleep(s/10000000);
					//  	
					printf("Child %d is created (sleep for %d seconds) \n",getpid(),s+2);
					sleep(s+2);
					
					// printf("Sleep of id %d is %d\n",getpid(),s);
					// break;	
					return 0;	
					// exit(1);
				}
			}
		}
		// while(c==0) continue;

		if (getpid() == ppid) {
			// while ((wpid = wait(&status)) > 0);
			while(c!=n);
			sleep(3);
			printf("Parent exited \n");
		}
	}
	else printf("Enter only the number of child processes to be created \n");
	return 0;
}