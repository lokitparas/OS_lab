/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
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
        error("ERROR opening socket");

     /* fill in port number to listen on. IP address can be anything (INADDR_ANY) */

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     /* bind socket to this port number on this machine */

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     /* listen for incoming connection requests */

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     /* accept a new request, create a newsockfd */

     int pid;
     while(1){
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
              error("ERROR on accept");

         /* read message from client */

        if ((pid = fork()) == -1)
        {
            close(newsockfd);
            continue;
        }
        else if(pid > 0)
        {
            close(newsockfd);
            printf("here2\n");
            continue;
        }
        else if(pid == 0)
        {
            printf("here3\n");
             bzero(buffer,256);
             n = read(newsockfd,buffer,255);
             if (n < 0) error("ERROR reading from socket");

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


             printf("Here is the message: %s\n",filename);

             /* send reply to client */
             FILE *filed = fopen(files_folder,"rb");
                if(filed==NULL)
                {
                    printf("File opern error");
                    return 1;   
                }
                printf("File opened");
                FILE *fp;
                fp = fopen("Output1.txt", "w");

                /* Read data from file and send it */
                while(1)
                {
                    /* First read file in chunks of 256 bytes */
                    unsigned char buff[512]={0};
                    bzero(buff,512);
                    int nread = fread(buff,1,512,filed);
                    printf("Bytes read %d \n", nread);        

                    /* If read was success, send data. */
                    if(nread > 0)
                    {
                        printf("Sending \n");
                        write(newsockfd, buff, nread);
                        fprintf(fp, "%s", buff);

                    }

                    /*
                     * There is something tricky going on with read .. 
                     * Either there was error, or we reached end of file.
                     */
                     printf("%d\n", nread);
                    if (nread < 512)
                    {   
                        if (feof(filed))
                            printf("End of file\n");
                        if (ferror(filed))
                            printf("Error reading\n");
                        break;
                    }


                }
            }
            close(newsockfd);
            break;
        }


     //n = write(newsockfd,"I got your message",18);
     //if (n < 0) error("ERROR writing to socket");
     return 0; 
}