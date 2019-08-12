#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
int flag = 0;

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

void sig_handler(int s){
	write(0,"\n\n",strlen("\n\n"));
}

void sig_handler1(int s){
    // printf("a.......|||||||||||\n");
    if(flag == 1) {
	    pid_t pid = wait(NULL);
        printf("Child %d has terminated\n",pid);
    }
}

void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;

    if (signal(SIGCHLD,sig_handler1) == SIG_ERR) printf("SIGCHLD Error");
    if (signal(SIGINT,sig_handler) == SIG_ERR) printf("SIGINT Error");

    while (1) {           
       
        printf("Hello>");     
        bzero(line, MAX_INPUT_SIZE);
        fgets(line, 1024, stdin);
        // gets(line);           
        // printf("Got command %s\n", line);
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
   
        //do whatever you want with the commands, here we just print them
        if (tokens[0] != NULL) {
            if (!strcmp(tokens[0],"exit")) exit(0);
            char **args = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
            int c =0 ;
            flag = 0;
            for (i=0;tokens[i]!=NULL;i++){

                if (!strcmp(tokens[i],"&&")){               
                    flag = 1;
                    pid_t a = fork();
                    if (a == -1) {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }                    

                    if (!a) {
                    	if (execvp(args[0],args) == -1) {
                        printf("%d error: %s not found\n",getpid(),args[0]);
                        }
                    }
                    for(int j=0;args[j]!=NULL;j++){
                        // printf("%s\n",args[j]);
                        free(args[j]);
                    }
                    // printf("\n");
                    free(args);
                    c=0;
                    char **args = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));                
                }
                else{
                    args[c] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                    strcpy(args[c++],tokens[i]);
                }
            }
            //do whatever you want with the commands, here we just print them
            if (c) {
                // printf("nope............%s\n",args[0]);
                args[c] = '\0'; 
                pid_t a = fork();
                if (a == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }                    

                if (!a) {
                    if (execvp(args[0],args) == -1) {
                        printf("%d error: %s not found\n",getpid(),args[0]);
                    }
                }else {
                    if (flag == 0){
                        pid_t p = wait(NULL);
                        printf("Child %d has terminated\n",p);

                    }
                }
                for(int j=0;args[j]!=NULL;j++){
                // printf("%s\n",args[j]);
                free(args[j]);
                }
            }
            
            // printf("\n");
            free(args);
            // Freeing the allocated memory	
        }
        // Freeing the allocated memory	
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
     

}

                
