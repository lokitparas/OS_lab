#include  <stdio.h>
#include  <sys/types.h>
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

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
		tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
		strcpy(tokens[tokenNo++], token);
		tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void command_exec(char** args){
	pid_t pid;

	pid = fork();
    if(pid == -1){
      perror ("fork");
      return;
    }
    if(pid == 0){
    	// char *args[] = { cmd, NULL };
   		int n;
   		n = execve(args[0], args, NULL);
   		if(n == -1){printf("error\n"); return;}
    }
    else{ 
    	wait(NULL);
    }
}


void  main(void)
{
     char  line[MAX_INPUT_SIZE];            
     char  **tokens;              
     int i;

     while (1) {           
       
       printf("Hello>");     
       bzero(line, MAX_INPUT_SIZE);
       gets(line);           
       printf("Got command %s\n", line);
       line[strlen(line)] = '\n'; //terminate with new line
       tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them

       for(i=0;tokens[i]!=NULL;i++){
	 		printf("found token %s\n", tokens[i]);
       }
       if ((strcmp(tokens[0], "ls") == 0 ) && tokens[1] == NULL) {
     		char *args[] = {"/bin/ls", NULL};
     		command_exec(args);  		
       }
       if ((strcmp(tokens[0], "cat") == 0 ) && tokens[1] != NULL && tokens[2] == NULL) {	
       		char *args[] = {"/bin/cat", tokens[1], NULL};
     		command_exec(args);  		
       }

       if ((strcmp(tokens[0], "echo") == 0 ) && tokens[1] != NULL) {
       		int i=1;
       		char *temp[MAX_TOKEN_SIZE * MAX_NUM_TOKENS];
       		bzero(temp,MAX_TOKEN_SIZE * MAX_NUM_TOKENS);
       		while(tokens[i] != NULL){
       			strcat(temp, tokens[i]);
				strcat(temp, " ");
				i++;
       		}

       		char *args[] = {"/bin/echo", temp, NULL};
     		command_exec(args);  		
       }

       // Freeing the allocated memory	
       for(i=0;tokens[i]!=NULL;i++){
		 free(tokens[i]);
       }	
       free(tokens);
     }
     

}

                
