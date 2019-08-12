#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void sig_handler(int signo)
{
  	if (signo == SIGINT || signo == SIGTERM) {

    	// printf("received SIGINT\n");
  		if(signo == SIGINT ) printf("Interrupt SIGINT received.\n");
  		else printf("Interrupt SIGTERM received.\n");
		printf("Do you really want to quit (y|n)?\n");
		while(1){
			char buf;
			scanf(" %c",&buf);
			// printf("%c\n", buf);
			fflush(stdin);
			if (buf == 'y') {
				exit(0);
				break;
			}
			if (buf == 'n') {
				printf("\n");
				// printf("Not exited %c\n",buf);
				break;
			}
		}
	}
	
}

int main(void)
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
  printf("\ncan't catch SIGINT\n");
  if (signal(SIGTERM, sig_handler) == SIG_ERR)
  printf("\ncan't catch SIGINT\n");
  // A long long wait so that we can easily issue a signal to this process
printf("%d\n", getpid());
  while(1) 
    sleep(1);
  return 0;
}