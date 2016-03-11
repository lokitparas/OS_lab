#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
pid_t parent_id;
pid_t group_pid;
int child_count=0;
int redirect= 0;
int sigint_called=0;
char server_IP[50];
char Port[10];
int server_info =0;

void sig_handler(int signo){
  
  if (signo == SIGINT){
    sigint_called =1;
    if(child_count != 0){
        if(kill(-group_pid, signo) == -1){
          perror("Error signal:");
        }
        return;
    }
  }
}

void sigquit_handler (int sig) {
    assert(sig == SIGQUIT);
    pid_t self = getpid();
    if (parent_id != self) { _exit(0);}
}

void my_sigchld_handler(int sig)
{ 
    pid_t p;
    int status;

    while ((p=waitpid(-1, &status, WNOHANG)) != -1)
    {  
        if(p != 0)printf("Pid %d exited.\n", p);     
        // if(p==0) break;
        /* Handle the death of pid p */
    }
    // printf("Pid %d exited.\n", group_pid);
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

void pipe_exec(char ** tokens){
  pid_t pid, pid2;
  pid_t pgid;
  int tmp;
  int i;  
  
  char* output;
  int n;
  // if(server_info == 0){ printf("No Server Info Available\n"); return;}
  char *args[] = {"get-one-file-sig", tokens[1], server_IP, Port, "display",NULL};
  
    char **tokens2 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));  // for storing the tokens for command on right side of pipe
    bzero(tokens2, MAX_NUM_TOKENS);
   int q;
  for(q=3; tokens[q]!=NULL;q++){
        tokens2[q-3] = tokens[q];
    }


  if (strcmp(tokens[0], "getfl") == 0){
          if(tokens[1] != NULL){
            if(server_info == 0){ printf("No Server Info Available\n"); return;}    
          }

          else{
                printf("server : wrong arguments\n");
            return;
          }     
    }

    /* creating pipe */
  int pfds[2];    
  pipe(pfds);
child_count++;
  pid = fork();     // forking for the left side command
  
    if(pid == -1){
      perror ("fork");
      return;
    }
    if(pid == 0){
      //child 1 body
      if(child_count == 1){group_pid = getpid();}
      tmp = setpgid(getpid(), group_pid);
      if(tmp ==-1){
        printf("pid3 setpgid error.\n");
      }
      
      close(1);       /* close normal stdout */
      dup(pfds[1]);   /* make stdout same as pfds[1] */
      close(pfds[0]); /* we don't need this */
      
      n = execve(args[0],args,NULL);
      
      if(n == -1){perror("Command Execution Error"); exit(0);}   
    }

    else{ 
      //parent body
       group_pid = pid;
      child_count++;
      pid2 = fork();    // forking for the right side command
     
      if(pid2 == -1){
         perror ("fork");
        return;
      }

      if(pid2 == 0){
        //child 2 body
        if(child_count == 1){group_pid = getpid();}
        tmp = setpgid(getpid(), group_pid);
        if(tmp ==-1){
          printf("pid3 setpgid error.\n");
        }
        close(0);       /* close normal stdin */
        dup(pfds[0]);   /* make stdin same as pfds[0] */
        close(pfds[1]); /* we don't need this */
        n = execvp(tokens2[0],tokens2);      
      }
      else{
        //parent body
        wait(NULL);    
      }
    
  }

}

void command_exec(char** tokens, int wait_stat){

	pid_t pid;
  pid_t pgid;
  int background =0 ;   // set to 1 if program is to be run in background
  int tmp;
  int getfile = 0;      // set to 1 if command is to get some file.
  int i;  
  
  char* output;
  int n;
  // if(server_info == 0){ printf("No Server Info Available\n"); return;}
  char *args[] = {"get-one-file-sig", tokens[1], server_IP, Port, "display",NULL};    // with display
  char *args2[] = {"get-one-file-sig", tokens[1], server_IP, Port, "nodisplay",NULL};   //without display

  if (strcmp(tokens[0], "getfl") == 0){
          if(tokens[1]!=NULL && tokens[2] == NULL)
            getfile = 1;      // getfile without redirection

          else if(tokens[1] != NULL &&  (strcmp(tokens[2],">") == 0) && tokens[3] != NULL && tokens[4] == NULL){
            getfile = 1;
            redirect = 1;   // redirection 
            output = tokens[3];
          }
          else{
                printf("Command : wrong arguments\n");
            return;
          }     
    }

    if ((strcmp(tokens[0], "getsq") == 0) || (strcmp(tokens[0], "getbg") == 0)) {
          getfile = 2;        // not a normal getfile command 

          if((strcmp(tokens[0], "getbg") == 0 )) { 
            background=1;     // background command issued
          }
          if(tokens[1] != NULL && tokens[2] == NULL){
            //correct arguments supplied;
          }
          else{
                printf("Command : wrong arguments\n");
            return;
          }     
    }
if(getfile > 0)if(server_info == 0){ printf("No Server Info Available\n"); return;};
    
 if(background ==0) child_count++;     // since child count maintains no of foreground processes 
 
	pid = fork();

    if(pid == -1){
      perror ("fork");
      return;
    }
    if(pid == 0){
      //child starts

      if(child_count == 1){group_pid = getpid();}   // first foreground process, so it defines the group id of all parallel getfile processes
      if(background ==0) { tmp = setpgid(getpid(), group_pid);}    // group id set for all foreground processes     
      else{
        
        tmp = setpgid(getpid(), parent_id);}     // all background processes have group pid as parent pid

      if(tmp ==-1){
        printf("pid setpgid error.\n");
      }
      
      // redirection
   		if(redirect == 1){
      
        int fd1 = creat(output, 0644);
        dup2(fd1, STDOUT_FILENO);
        close(fd1);
      }

      if( getfile == 0){
        n = execvp(tokens[0], tokens);    // for commands except all get file commands
      }
      else{
        if(getfile == 1)
          n = execve(args[0],args,NULL);  // if it is simple getfile command

        if(getfile == 2)
          n = execve(args2[0],args2,NULL);  // for variants of getfile command ( getsq and getbg)
      }
   		if(n == -1){perror("Command Execution Error"); exit(0);}
    }

    else{ 

      //parent body

      if(child_count == 1) group_pid = pid;
      if(background == 1){ printf("Process %d started in background\n", pid );}
      
      if(wait_stat == 1){
    	   wait(NULL);       // only waits if it is foreground process and not parallel execution
       }
       return;
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
  if(tokens[0] == NULL) return;
  child_count = 0;


  int pipe_pos =-1;
  int i;
    for(i=0;tokens[i]!=NULL;i++){
       if(strcmp(tokens[i],"|") == 0){
        pipe_pos = i;
       }
    }

    if(pipe_pos == -1){
        
        if ((strcmp(tokens[0], "cd") == 0 )) { 
            /* command found is cd */ 
             if(tokens[1] != NULL && tokens[2] == NULL){
                cd(tokens[1]);      //calling cd    
             }
              else{
                printf("cd : wrong arguments\n");
                return;
              }
        }

        else if ((strcmp(tokens[0], "server") == 0 )) {  
           if(tokens[1] != NULL && tokens[2] != NULL && tokens[3] == NULL){
              strcpy(server_IP,tokens[1]);
              strcpy(Port,tokens[2]);
              server_info = 1;
           }
            else{
              printf("server : wrong arguments\n");
            }
        }
        
        else if ((strcmp(tokens[0], "getsq") == 0 )) {
            // getsq is nothing but getfl command executed one after the other
              if(tokens[1] == NULL){
                printf("Wrong arguments supplied\n");
                return;
              }
              char **tokens2 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));    // so tokens2 contains arguments for only one file at a time
              bzero(tokens2, MAX_NUM_TOKENS);
              tokens2[0] = tokens[0];
              int j=1;
              while(tokens[j] != NULL){     // getting entire file one after the other for all the filenames provided
                   
                  tokens2[1] = tokens[j];
                  tokens2[2] = NULL;
                  command_exec(tokens2,1);
                  child_count = 0;   
                   if(sigint_called == 1) {break;}
                   printf("%s Downloaded\n",tokens[j] );
                  j++;
              }
              int w;
               for(w=0;tokens2[w]!=NULL;w++){
                 free(tokens2[w]);
              } 
              free(tokens2);
        }
        else if ((strcmp(tokens[0], "getpl") == 0 )) {
              if(tokens[1] == NULL){
                printf("Wrong arguments supplied\n");
                return;
              }
              char **tokens2 = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
              bzero(tokens2, MAX_NUM_TOKENS);
              tokens2[0] = "getsq";
              int j=1;
              while(tokens[j] != NULL){
                  printf("Started Downloading %s\n",tokens[j] );
                  tokens2[1] = tokens[j];
                  tokens2[2] = NULL;
                  command_exec(tokens2,0);
                  j++;
                  printf("gpid -> %d\n",group_pid);
              }
               wait(NULL);
              // waitpid(-group_pid, NULL,0);

              child_count=0;
         return;     
        }
        else if ((strcmp(tokens[0], "getbg") == 0 )) {
              command_exec(tokens,0);
         return;     
        }
        else if(strcmp(tokens[0], "exit") == 0 && tokens[1] == NULL){
          kill(-parent_id, SIGQUIT);
          if(waitpid(-1,NULL,WNOHANG) > 0){
             printf("Process completed execution\n");
          }
           exit(0);
        }
        else{
          command_exec(tokens,1);
          child_count = 0;
        } 
      }
      else{
        //pipe
          if((strcmp(tokens[0], "getfl") == 0) && tokens[1] != NULL && (strcmp(tokens[2], "|") == 0 )) {
            pipe_exec(tokens);
            child_count = 0;
          }
          else{
            printf("Wrong arguments supplied\n");
            return;
          }
      }
}






void  main(void){
  parent_id = getpid();
  char  line[MAX_INPUT_SIZE];            
  char  **tokens;              
  int i;
  int status;
  bzero(server_IP, 50);
  bzero(Port, 10);
  if (signal(SIGINT, sig_handler) == SIG_ERR)printf("\ncan't catch SIGINT\n");
  // signal(SIGCHLD, handler);

  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = my_sigchld_handler;

  sigaction(SIGCHLD, &sa, NULL);

  signal(SIGQUIT, sigquit_handler);


  while (1){
    child_count = 0;
    sigint_called = 0;
    // while ((p=waitpid(-parent_id, &status, WNOHANG)) != -1)
    // {
    //     printf("Pid %d exited................\n", p);     
    //     if(p==0) break;
    // }
    printf("Hello>");     
    bzero(line, MAX_INPUT_SIZE);
    gets(line);           
    line[strlen(line)] = '\n'; //terminate with new line
    tokens = tokenize(line);

    fun_detector(tokens);
    }
   // Freeing the allocated memory	
    for(i=0;tokens[i]!=NULL;i++){
       free(tokens[i]);
    }	
    free(tokens);
  }


                
