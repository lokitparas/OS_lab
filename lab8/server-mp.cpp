/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <cstdlib>
#include "queue.c"
using namespace std;

int num_threads;
int queue_size;
int empty;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
		

void error(char *msg)
{
    perror(msg);	
    exit(1);
}
void *worker_thread(void *socket_desc);

int main(int argc, char *argv[])
{	
    Queue *Q = createQueue(5);
    Enqueue(Q,1);
    printf("Front element is %d\n",front(Q));
    int status, w;
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
    }

     /* create socket */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket server");

     /* filling in port number to listen on */

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); 
    num_threads = atoi(argv[2]);
    queue_size = atoi(argv[3]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

     /* bind socket to this port number on this machine */

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

    int queue[queue_size];
    bzero(queue,queue_size*4);
    empty=0;
     
     /* listen for incoming connection requests */

    // pthread_t sniffer_thread[num_threads];
    // for (i=0; i<num_threads; i++) {
    //     if( pthread_create( &sniffer_thread[i] , NULL , worker_thread , (void*) &i) < 0)
    //     {
    //         perror("could not create thread");
    //         return 1;
    //     }
    // }

    

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     int pid;
     while(1){
		     /* accept a new request, create a newsockfd */     	
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
              error("ERROR on accept");

         while((w = waitpid(-1, 0, WNOHANG)) > 0){
                kill(w,SIGKILL);
            }
         
        if ((pid = fork()) == -1)
        {
            close(newsockfd);
            continue;
        }
        else if(pid > 0)
        {   
            close(newsockfd);
            continue;
        }
        else if(pid == 0)
        {	/* read message from client */

             bzero(buffer,256);
             n = read(newsockfd,buffer,255);
             if (n < 0) error("ERROR reading from socket");

             /* extracting filename */
             char filename[20];
             bzero(filename,20);
               int i=0;
               for(i=0;i<20;i++){
                    filename[i] = buffer[i+4];
                    if(filename[i] == '.'){
                        filename[i+1] = buffer[i+5];
                        filename[i+2] = buffer[i+6];
                        filename[i+3] = buffer[i+7];
                        break;
                    }
               }
            char files_folder[] = "./files/";
            strcat(files_folder, filename);

             /* send reply to client */
             FILE *filed = fopen(files_folder,"rb");
                if(filed==NULL)
                {
                    printf("File opern error");
                    return 1;   
                }
                

                /* Read data from file and send it */
                while(1)
                {
                    /* First read file in chunks of 512 bytes */
                    unsigned char buff[512]={0};
                    bzero(buff,512);
                    int nread = fread(buff,1,512,filed);
                            
                    /* If read was success, send data. */
                    if(nread > 0)
                    {
                        write(newsockfd, buff, nread);
                        
                    }

                    /* Either there was error, or we reached end of file. */
                     
                    if (nread < 512)
                    {   
                        if (feof(filed))
                            // printf("End of file\n");
                        if (ferror(filed))
                            printf("Error reading\n");
                        break;
                    }


                }
                fclose(filed);
		}
          close(newsockfd);
	  exit(0);     

    }
     return 0; 
}

// void *worker_thread(void *threadid){
// 	while(1){
// 		int* threadnum = (int*)threadid;  
// 	    char buffer[512];
// 	    Pthread_mutex_lock(&mutex);
// 	    while(empty == 0){
// 	    	Pthread_cond_wait(&queue_not_empty, &mutex);
// 	    }

// 	    Pthread_mutex_unlock(&m);
// 	    /* create socket, get sockfd handle */
	    
	    
// 	}
// }
