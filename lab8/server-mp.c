/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "queue.c"
#include <pthread.h>

int num_threads;
int queue_size;
int empty;
int newsockfd;
Queue *Q;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_has_space = PTHREAD_COND_INITIALIZER;
		

void error(char *msg)
{
    perror(msg);	
    exit(1);
}
void *worker_thread(void *socket_desc);

int main(int argc, char *argv[])
{	
    int status, w,i,q,s;
    int sockfd, portno, clilen;
    void* res;
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

    int array[num_threads];
    for (i = 0; i < num_threads; ++i)
    {
    	array[i] = i;
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

    Q = createQueue(queue_size);
    empty=0;
     
     /* listen for incoming connection requests */

    pthread_t sniffer_thread[num_threads];
    for (i=0; i<num_threads; i++) {
        if( pthread_create( &sniffer_thread[i] , NULL , worker_thread , (void*) &array[i]) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }

  
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     int pid;
     while(1){
		     /* accept a new request, create a newsockfd */     	
        pthread_mutex_lock(&mutex);
        	while(queueisfull(Q) == 1){
        		pthread_cond_wait(&queue_has_space, &mutex);
        	}
        pthread_mutex_unlock(&mutex);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
              error("ERROR on accept");
        pthread_mutex_lock(&mutex);
        	Enqueue(Q,newsockfd);
        	printf("New newsockfd enqueue %d\n",newsockfd );
        	pthread_cond_signal(&queue_not_empty);
        pthread_mutex_unlock(&mutex);
         
	}

	for (q = 0; q < num_threads; ++q)
    {	s = pthread_join(sniffer_thread[q],&res);
    	if(s != 0){
    		perror("Error Joining\n");
    	}
    }

    pthread_exit(NULL);
}

void *worker_thread(void *threadid){
	int* threadnum = (int*)threadid;
	while(1){
		pthread_mutex_lock(&mutex);
	    while(sizeofqueue(Q) == 0){
	    	printf("Sleeping %d\n",*threadnum);
	    	pthread_cond_wait(&queue_not_empty, &mutex);
	    	printf("Waking up %d\n",*threadnum);
	    }
	    int sockfd = front(Q);
	    printf("got front %d\n",*threadnum);
	    // printf("newsockfd %d for %d\n",newsockfd,*threadnum);
	    Dequeue(Q);
	    pthread_cond_signal(&queue_has_space);
	    printf("Dequeue %d\n",*threadnum);
	    pthread_mutex_unlock(&mutex);
	    //sleep(10);
	    /* create socket, get sockfd handle */
	    char buffer[256];
	    int n;
	    bzero(buffer,256);
             n = read(sockfd,buffer,255); 
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
                    return ;   
                }
                

                 // Read data from file and send it 
                while(1)
                {
                    /* First read file in chunks of 512 bytes */
                    unsigned char buff[512]={0};
                    bzero(buff,512);
                    int nread = fread(buff,1,512,filed);
                    // printf("writing %d\n",nread );    
                    /* If read was success, send data. */
                    if(nread > 0)
                    {	
                        write(sockfd, buff, nread);
                        
                    }

                    /* Either there was error, or we reached end of file. */
                     
                    if (nread < 512)
                    {   
                        if (feof(filed))
                             printf("End of file\n");
                        if (ferror(filed))
                            printf("Error reading\n");
                        break;
                    }


                }
                fclose(filed);
                printf("closing 1 %d\n" , sockfd);
                close(sockfd);
                printf("closing 2 %d\n", sockfd);
		}
	    
	}
