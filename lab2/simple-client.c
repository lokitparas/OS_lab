#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[512];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    /* create socket, get sockfd handle */

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address in sockaddr_in datastructure */

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
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

    printf("Please enter the message: ");
    bzero(buffer,512);
    fgets(buffer,511,stdin);

    /* send user message to server */

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,512);
    /* read reply from server */
    FILE *fp;
    fp = fopen(argv[3], "w");// "w" means that we are going to write on this file
    
    while(1){
    bzero(buffer,512);
    n = read(sockfd,buffer,511);
    printf("%d\n",n);
    if(n==0){printf("end of file\n"); break;}
    if (n < 0) {
         error("ERROR reading from socket");
         break;
     }
    //printf("%s",buffer);
     if(n>0){
        fprintf(fp, "%s", buffer);
      }
    }
    printf("\n");
    return 0;
}
