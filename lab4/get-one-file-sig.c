#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>
#include <unistd.h>
int bytes_downloaded;
void sig_handler(int signo){
	if (signo == SIGINT)
		printf("Received SIGINT; downloaded %d bytes so far.\n", bytes_downloaded);
	exit(0);
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	
	bytes_downloaded = 0;
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[512];
    if (argc < 5) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    if((strcmp(argv[4], "display") != 0)&&(strcmp(argv[4], "nodisplay") != 0)){
       fprintf(stderr,"incorrect mode\n");
       exit(0);
    }

    /* create socket, get sockfd handle */

    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address in sockaddr_in datastructure */

    server = gethostbyname(argv[2]);
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


        /* ask user for filename */
    int r = rand() % 9999;
    bzero(buffer,512);
    int r1 = 0;
    sprintf(buffer, "get %s", argv[1]);
    //printf("%s\n",buffer);
       
    /* send user message to server */

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");


    bzero(buffer,512);
    while(1){
        /*Checking for signals*/
    	if (signal(SIGINT, sig_handler) == SIG_ERR)
    		printf("\ncan't catch SIGINT\n");

	    bzero(buffer,512);
	    n = read(sockfd,buffer,511);
	    bytes_downloaded = bytes_downloaded + n;
	    //printf("%d\n",n);
	    if(n==0){
	        //printf("end of file\n");
	        break;
	    }
	    if (n < 0) {
            printf("fkjaghalkgse\n");
	         error("ERROR reading from socket");
	         break;
	     }
	    if(strcmp("display",argv[4]) == 0)
	        printf("%s",buffer);
    }
    // printf("\n");
    return 0;
}
