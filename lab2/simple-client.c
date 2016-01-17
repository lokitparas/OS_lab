#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#define NUM_CLIENT 5

int portno;
struct hostent *server;
void error(char *msg)
{
    perror(msg);
    exit(0);
}
void *connection_handler(void *socket_desc);
int main(int argc, char *argv[])
{
    
    pthread_t sniffer_thread;
    int i;
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);

    for (i=1; i<=NUM_CLIENT; i++) {
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) i) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        sleep(3);
    }
    pthread_exit(NULL);
    return 0;
}

void *connection_handler(void *threadid){
	printf("%p\n",threadid);
	int sockfd,n;
	int threadnum = (int)threadid;
    struct sockaddr_in serv_addr;
    

    char buffer[512];
    

    /* create socket, get sockfd handle */

    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address in sockaddr_in datastructure */

    
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    printf("%d\n",portno);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    /* connect to server */

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    /* ask user for input */
    int r = rand() % 9999;
	bzero(buffer,512);
	sprintf(buffer, "get foo%d.txt", r);
	printf("%s\n",buffer);
    //printf("Please enter the message: ");
    //bzero(buffer,512);
    //fgets(buffer,511,stdin);

    /* send user message to server */

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,512);
    /* read reply from server */
    // FILE *fp;
    // fp = fopen(argv[3], "w");// "w" means that we are going to write on this file
    
    while(1){
    bzero(buffer,512);
    n = read(sockfd,buffer,511);
    //printf("%d\n",n);
    if(n==0){printf("end of file\n"); break;}
    if (n < 0) {
         error("ERROR reading from socket");
         break;
     }
    //printf("%s",buffer);
     if(n>0){
        // fprintf(fp, "%s", buffer);
      }
    }
    close(sockfd);
    printf("\n");
    return 0;
}
