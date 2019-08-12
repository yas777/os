#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc == 2){
		int n = atoi(argv[1]),c=0,status=0;
		// printf("%d\n",n);
		pid_t ppid = getpid(),wpid;
		printf("Parent is %d\n",getpid());
			
		printf("Number of children is %d\n",n);
		srand(10);
		pid_t a[n];
		for (int i=0;i<n;i++){
			if (i==n) c=1;
			if (i<n) {
				a[i] = fork();
				if (a[i] && getpid() == ppid){
					printf("Child %d is created \n",a[i]);
					// wait(NULL);
					continue;
				}
				else{
					srand(getpid());
					int s = (rand()) % 20;
					// sleep(s/10000000);
					// 
					sleep(s+2);
					// printf("Sleep of id %d is %d\n",getpid(),s);
					printf("Child %d exited \n",getpid()); 
					// break;
					exit(1);
				}
			}
		}
		// while(c==0) continue;

		if (getpid() == ppid) {
			while ((wpid = wait(&status)) > 0);
			printf("Parent exited \n");
		}
		// printf("sdthsdgnj %d\n",getpid());

	}
	else {
		printf("Enter only the number of child processes to be created \n");
	}
	return 0;
}