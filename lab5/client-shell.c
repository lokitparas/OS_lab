#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
pid_t parent_id;
pid_t group_pid;

void sig_handler(int signo){

  if (signo == SIGINT){
      // printf("in here %d : %d\n", getpid(), parent_id);
      if(killpg(3000, signo) == -1){
        printf("%d\n", group_pid);
        perror("Error signal:");
      }
  }
  return;
}


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
  pid_t pgid;
  int tmp;
	pid = fork();
    if(pid == -1){
      perror ("fork");
      return;
    }
    if(pid == 0){
      tmp = setpgid(pid, 3000);
      printf("%d\n", getpid());
      printf("%d\n", getpgid(getpid()));
      if(tmp ==-1){
        printf("pid3 setpgid error.\n");
      }
   		int n;
   		n = execve(args[0], args, NULL);
   		if(n == -1){perror("Error"); return;}
    }
    else{ 
    	wait(NULL);
    }
}


int cd(char *pth){
   // char path[BUFFERSIZE];
   // strcpy(path,pth);

   // char *token;

   char cwd[PATH_MAX + 1]; 
   if(pth[0] != '/')
   {// true for the dir in cwd
    getcwd(cwd,PATH_MAX + 1);
    strcat(cwd,"/"); 
    strcat(cwd,pth);
    if(chdir(cwd) == -1){
      perror("Error in cd");
    } 
   }else{//true for dir w.r.t. /
    chdir(pth);
   }

   return 0;
}

void  main(void){
  group_pid = 3000;
  parent_id = getpid();
  char  line[MAX_INPUT_SIZE];            
  char  **tokens;              
  int i;
  char server_IP[50];
  bzero(server_IP, 50);
  char Port[10];
  bzero(Port, 10);
  while (1){
    if (signal(SIGINT, sig_handler) == SIG_ERR)printf("\ncan't catch SIGINT\n");
    printf("Hello>");     
    bzero(line, MAX_INPUT_SIZE);
    gets(line);           
    printf("Got command %s\n", line);
    line[strlen(line)] = '\n'; //terminate with new line
    tokens = tokenize(line);
 
     //do whatever you want with the commands, here we just print them

    //  for(i=0;tokens[i]!=NULL;i++){
 		  // printf("found token %s\n", tokens[i]);
   //  }
    if ((strcmp(tokens[0], "ls") == 0 ) && tokens[1] == NULL) {
   		 char *args[] = {"/bin/ls", NULL};
   		 command_exec(args);  		
    }
    if ((strcmp(tokens[0], "cat") == 0 ) && tokens[1] != NULL && tokens[2] == NULL) {	
     		char *args[] = {"/bin/cat", tokens[1], NULL};
   		   command_exec(args);  		
    }
    if ((strcmp(tokens[0], "cd") == 0 )) {  
         if(tokens[1] != NULL && tokens[2] == NULL){
            cd(tokens[1]);    
         }
          else{
            printf("cd : wrong arguments\n");
          }
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

    if ((strcmp(tokens[0], "server") == 0 )) {  
       if(tokens[1] != NULL && tokens[2] != NULL && tokens[3] == NULL){
          strcpy(server_IP,tokens[1]);
        //server_IP = gethostbyname(tokens[1]);
          strcpy(Port,tokens[2]);
       }
        else{
          printf("server : wrong arguments\n");
        }
    }
    if (strcmp(tokens[0], "getfl") == 0){
      if(tokens[1] != NULL && tokens[2] == NULL){ 
        char *args[] = {"get-one-file-sig", "files/foo0.txt", "127.0.0.1", "4000", "display", NULL};
        // char *args[] = {"get-one-file-sig", tokens[1], server_IP, Port, "display", NULL};
        command_exec(args); 
      }
      else{
        printf("server : wrong arguments\n");
      }     
    }


   // Freeing the allocated memory	
    for(i=0;tokens[i]!=NULL;i++){
       free(tokens[i]);
    }	
    free(tokens);
  }
}

                
