#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

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


void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;

    while (1) {           
       
        printf("Hello>");     
        bzero(line, MAX_INPUT_SIZE);
        fgets(line, 1024, stdin);
        // gets(line);           
        // printf("Got command %s\n", line);
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);

        if (tokens[0] != NULL) {
            char **temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
            int c =0 ;
            for (i=0;tokens[i]!=NULL;i++){
                if (!strcmp(tokens[i],"|")){
                    close(1);

                    int pipefd[2];
                    if (pipe(pipefd) == -1) {
                        perror("pipe");
                        exit(EXIT_FAILURE);
                    }

                    printf("ids %d ..... %d",pipefd[0],pipefd[1]);
                    // pid_t a = fork();
                    // if (cpid == -1) {
                    //     perror("fork");
                    //     exit(EXIT_FAILURE);
                    // }

                    // if (a) {
                    //     // close(pipefd[1]);
                    //     // read();
                    //     // close(pipefd[0]);
                    //     // wait(NULL);

                    // }
                    // else {
                    //     pid_t a1 = fork();
                    //     if (a1){

                    //     }else {
                    //         // execvp(args[0],args);
                    //     }
                    // }

                    for(int j=0;temp[j]!=NULL;j++){
                        printf("%s\n",temp[j]);
                        free(temp[j]);
                    }
                    printf("\n");
                    free(temp);
                    c=0;
                    char **temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));                
                }
                else{
                    temp[c] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                    strcpy(temp[c++],tokens[i]);
                }
            }
            //do whatever you want with the commands, here we just print them
            for(int j=0;temp[j]!=NULL;j++){
                        printf("%s\n",temp[j]);
                        free(temp[j]);
                    }
                    printf("\n");
            free(temp);
            // Freeing the allocated memory	
        }
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
     

}

                
