#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
pid_t parent_id;
pid_t group_pid;
int child_count=0;
int redirect= 0;
void sig_handler(int signo){

  if (signo == SIGINT){
       // printf("in here %d : %d\n", getpid(), parent_id);
      if(killpg(group_pid, signo) == -1){
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

void command_exec(char** tokens){
	pid_t pid;
  pid_t pgid;
  int tmp;
  int getfile = 0;
   
  char* output;
  int n;
  // char* args[6];
  char *args[] = {"get-one-file-sig", "files/foo0.txt", "127.0.0.1", "4001", "display",NULL};

  if ((strcmp(tokens[0], "ls") == 0 )) {
           tokens[0] = "/bin/ls";     
        }

  if ((strcmp(tokens[0], "cat") == 0 )) { 
            tokens[0] = "/bin/cat";     
        }
  if ((strcmp(tokens[0], "grep") == 0 ) ) {  
            tokens[0] = "/bin/grep";     
        }
  if ((strcmp(tokens[0], "echo") == 0 )) {
            tokens[0] = "/bin/echo";      
        }
  if (strcmp(tokens[0], "getfl") == 0){
          getfile = 1;
          printf("qwertyu\n");  
          if(tokens[1] != NULL && tokens[2] == NULL){ 
              
          }

          else if(tokens[1] != NULL &&  (strcmp(tokens[2],">") == 0) && tokens[3] != NULL && tokens[4] == NULL){
            
            redirect = 1;
            
          }
          else{
                printf("server : wrong arguments\n");
            return;
          }     
    }
  


	pid = fork();
  child_count++;
    if(pid == -1){
      perror ("fork");
      return;
    }
    if(pid == 0){
      if(child_count == 1){group_pid = getpid();}
      tmp = setpgid(getpid(), group_pid);
       // printf("My pid %d\n", getpid());
       // printf("Group pid %d\n", getpgid(getpid()));
      if(tmp ==-1){
        printf("pid3 setpgid error.\n");
      }
   		
      // redirection
   		if(redirect == 1){
        int fd1 = creat(output, 0644);
        dup2(fd1, STDOUT_FILENO);
        close(fd1);
      }

      if( getfile == 0){
        

        n = execvp(tokens[0], tokens);
      }
      else{

        n = execve(args[0],args,NULL);
      }
   		if(n == -1){perror("Error"); return;}
    }

    else{ 
      group_pid = pid;
    	wait(NULL);
    }
}


int cd(char *pth){

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


void fun_detector(char** tokens){

  printf("in here\n");
  char server_IP[50];
  bzero(server_IP, 50);
  char Port[10];
  bzero(Port, 10);

  int pipe_pos =0;
  int i;
    for(i=0;tokens[i]!=NULL;i++){
       if(strcmp(tokens[i],"|") == 0){
        pipe_pos = i;
       }
    }

    if(pipe_pos == 0){
        // if ((strcmp(tokens[0], "ls") == 0 )) {
        //    char *args[] = {"/bin/ls", NULL};
        //    command_exec(tokens,0,NULL);     
        // }
        // if ((strcmp(tokens[0], "cat") == 0 ) && tokens[1] != NULL && tokens[2] == NULL) { 
        //     char *args[] = {"/bin/cat", tokens[1], NULL};
        //      command_exec(tokens,0,NULL);     
        // }
        // if ((strcmp(tokens[0], "grep") == 0 ) && tokens[1] != NULL && tokens[2] == NULL) {  
        //     char *args[] = {"/bin/grep", tokens[1], NULL};
        //      command_exec(tokens,0,NULL);     
        // }
        if ((strcmp(tokens[0], "cd") == 0 )) {  
             if(tokens[1] != NULL && tokens[2] == NULL){
                cd(tokens[1]);    
             }
              else{
                printf("cd : wrong arguments\n");
              }
        }
        // if ((strcmp(tokens[0], "echo") == 0 ) && tokens[1] != NULL) {
        //     int i=1;
        //     char *temp[MAX_TOKEN_SIZE * MAX_NUM_TOKENS];
        //     bzero(temp,MAX_TOKEN_SIZE * MAX_NUM_TOKENS);
        //     while(tokens[i] != NULL){
        //       strcat((char*)temp, tokens[i]);
        //       strcat((char*)temp, " ");
        //       i++;
        //     }

        //     char *args[] = {"/bin/echo", (char*)temp, NULL};
        //     command_exec(args,0,NULL);      
        // }

        if ((strcmp(tokens[0], "server") == 0 )) {  
           if(tokens[1] != NULL && tokens[2] != NULL && tokens[3] == NULL){
              strcpy(server_IP,tokens[1]);
              strcpy(Port,tokens[2]);
           }
            else{
              printf("server : wrong arguments\n");
            }
        }
        // printf("not cd and server\n");
        command_exec(tokens);
        child_count = 0;
        //getfl

        // if (strcmp(tokens[0], "getfl") == 0){
        //   if(tokens[1] != NULL && tokens[2] == NULL){ 
        //     char *args[] = {"get-one-file-sig", "files/foo0.txt", "127.0.0.1", "4001", "display", NULL};
        //     // char *args[] = {"get-one-file-sig", tokens[1], server_IP, Port, "display", NULL};
        //     command_exec(args,0,"Not Applicable"); 
        //     child_count = 0;
        //     // printf("%d\n", child_c ount);
        //   }
        //   if(tokens[1] != NULL &&  (strcmp(tokens[2],">") == 0) && tokens[3] != NULL && tokens[4] == NULL){
        //     char *args[] = {"get-one-file-sig", "files/foo0.txt", "127.0.0.1", "4001", "display",NULL};
        //     // char *args[] = {"get-one-file-sig", tokens[1], server_IP, Port, "display", NULL};
        //     command_exec(args,1,tokens[3]); 
        //     child_count = 0;
        //   }
        //   else{
        //     // printf("%s\n",tokens[2] );
        //     printf("server : wrong arguments\n");
        //   }     
        // }
      }
      else{
        //pipe 
           
      }
    


}






void  main(void){
  parent_id = getpid();
  char  line[MAX_INPUT_SIZE];            
  char  **tokens;              
  int i;
  if (signal(SIGINT, sig_handler) == SIG_ERR)printf("\ncan't catch SIGINT\n");
  while (1){
    
    printf("Hello>");     
    bzero(line, MAX_INPUT_SIZE);
    gets(line);           
    printf("Got command %s\n", line);
    line[strlen(line)] = '\n'; //terminate with new line
    tokens = tokenize(line);
  
    fun_detector(tokens);
     //do whatever you want with the commands, here we just print them

    //  for(i=0;tokens[i]!=NULL;i++){
 		  // printf("found token %s\n", tokens[i]);
   //  }
    
    }
   // Freeing the allocated memory	
    for(i=0;tokens[i]!=NULL;i++){
       free(tokens[i]);
    }	
    free(tokens);
  }


                
