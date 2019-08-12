#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64


void sig_handler(int signo)
{
  	if (signo == SIGINT || signo == SIGTERM) {

    	// printf("received SIGINT\n");
  		if(signo == SIGINT ) printf("Interrupt SIGINT received.\n");
  		else printf("Interrupt SIGTERM received.\n");
	}
	
}


char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}

void funccd(char **tokens,int* c1){
	if (!strcmp(tokens[0],"cd")){
				// printf("Entered cd\n");
				// printf("%s\n",tokens[2]);
		if (tokens[1] == NULL) printf("Wrong number of arguments\n");
		else{
			if (tokens[2] != NULL) c1=3;
			if (tokens[2] != NULL && (strcmp(tokens[2], ";;"))) printf("Wrong number of arguments\n");
			else {
				int dir = chdir(tokens[1]);
				if(dir != -1) printf("Entered directory %s\n",tokens[1]);
				else perror("Error");				
			}
		}
	}
}


void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
  	if (signal(SIGTERM, sig_handler) == SIG_ERR)
  		printf("\ncan't catch SIGTERM\n");

    while (1) {           
       	int c = 0,c1 = 0,incd = 0;
        printf("Hello>");     
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
        printf("Got command %s\n", line);
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
        char **args = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));

        // printf("%s\n", tokens[2]);

		
		if(tokens[0] != NULL){
			funccd(tokens,&c1);
			// printf("%d\n", c);
			if(strcmp(tokens[0],"cd") || c1!=0) {
				// printf("Entered ls\n");
				
				for(i=c1;tokens[i]!=NULL;i++) {
					// for (i=0;args[i]!=NULL;i++) printf("args %s\n",args[i]);
					if (!strcmp(tokens[i],"cd")) incd = 1;
					if (!strcmp(tokens[i],"exit")) {
						printf("Exited\n");
						exit(0);
					}
					if (!strcmp(tokens[i],";;") || !strcmp(tokens[i],">")) {
						args[c] = NULL;
						int filefd;
						if (!strcmp(tokens[i],">")) filefd = open(tokens[i+1], O_RDWR  | O_CREAT);
						if (incd) {
							funccd(args,&c1);
							incd = 0;
						}
						else {
							pid_t a = fork();
							if (a) {
								if (!strcmp(tokens[i],">")) {
									close(filefd);
									i++;
								}
								wait(NULL);
							
							}else{
								if (!strcmp(tokens[i],">")) {
									close(1);
									dup(filefd);
								}
								if(!execvp(args[0],args)){
									perror("Error");
								}
								return 0;
								// printf("%s\n", );
							}
						}
						c = 0;
						for(int j=0;args[j]!=NULL;j++){
						    free(args[j]);
						}
						free(args);		

						char **args = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));	

						// fflush(stdout);	
					}
					else {
						// printf("args update %d\n",i);
						args[c] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
						strcpy(args[c++] , tokens[i]);
						
					}
				}
				args[c] = NULL;
				// for (i=0;args[i]!=NULL;i++) printf("args %s\n",args[i]);
				// printf("out of \n");
				pid_t b =fork();
				if (b) {
					wait(NULL);
				}else{
					// printf("Entered child\n");
					if(!execvp(args[0],args)){
						perror("Error");
					}
					return 0;
					// printf("%s\n", );
				}
				for(int j=0;args[j]!=NULL;j++){
					free(args[j]);
				}
				free(args);	
			}
		}
        // Freeing the allocated memory	
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
  		
    }
     

}

                
