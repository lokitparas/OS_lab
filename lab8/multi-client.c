#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
typedef struct{
	int id;	// thread_id
    int total_requests;	//total requests made
	int total_completed;	//total requests completed
	float response_time;	// avg time taken for a request

    }stats;


int portno;		// server port number
int tot_time;	// total time for running the experiment
int sleep_time;	// Sleep time 
int global_time;	// overall time spent till now
int users;			// number of threads
char * mode;		// random or fixed mode
struct hostent *server;	
struct timeval start, end;	
stats* users_array;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void *connection_handler(void *socket_desc);
int main(int argc, char *argv[])
{
    int i,s;
    void *res;
    if (argc < 7) {					// insufficient arguments
       fprintf(stderr, "insufficient arguments\n");
       exit(0);
    }

    /* initializing variables */
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);
    users = atoi(argv[3]);
    tot_time = atoi(argv[4]);
    sleep_time = atoi(argv[5]);
   	
   	if(argv[6] != NULL){			// checking the mode as random or fixed
	   	if(strcmp("random",argv[6]) == 0){
			mode = argv[6];
		}
		else {
			if(strcmp("fixed",argv[6]) == 0){
				mode = argv[6];
			}
			else{
				printf("Invalid mode: use [random] or [fixed]\n");
				exit(0);
			}
		}
	}
	else{
		fprintf(stderr,"Mode not provided\n");
		exit(0);
	}	

   	 /* setting up the array to store information for each thread */
   	
   	users_array = malloc(users * sizeof(stats));
   	
   	if ( users_array == NULL ){
	    /* we have a problem */
	    printf("Error: out of memory.\n");
	    return;
	}
	int q;
	for (q = 0; q < users; ++q)
	{
	
		users_array[q].id = q;
		users_array[q].total_completed= 0;
		users_array[q].total_requests= 0;
		users_array[q].response_time = 0.0;
	}
	
  	gettimeofday(&start, NULL);  // start time for the experiment noted


	/* creating threads */
   	pthread_t sniffer_thread[users];
    for (i=0; i<users; i++) {
        if( pthread_create( &sniffer_thread[i] , NULL ,  connection_handler , (void*) &users_array[i].id) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }
    
    /* joining threads */
    for (q = 0; q < users; ++q)
    {	s = pthread_join(sniffer_thread[q],&res);
    	if(s != 0){
    		perror("Error Joining\n");
    	}
    }

    /* Calculating final avg statistics */
    int total_served =0;
    float total_res_time=0.0, throughput=0.0, avg_res_time=0.0;
    for (q = 0; q < users; ++q)
    {	total_served = total_served + users_array[q].total_completed;
    	total_res_time = total_res_time+ users_array[q].response_time;
    	printf("######################################\n");
	    printf("total_requests for %d: %d\n",q,users_array[q].total_requests);
	    printf("total_completed for %d: %d\n",q,users_array[q].total_completed);
	    printf("response_time for %d: %.2f\n",q,users_array[q].response_time);
	    printf("######################################\n");
	}

	throughput = total_served/tot_time;
	avg_res_time = total_res_time/users;
	printf("///////////////////////////////////////\n");
	printf("///////////////////////////////////////\n");
	printf("///////////////////////////////////////\n");
	printf("%.2f\n",throughput);
	printf("%.2f\n",avg_res_time);
	printf("///////////////////////////////////////\n");
	printf("///////////////////////////////////////\n");
	printf("///////////////////////////////////////\n");
	
    pthread_exit(NULL);
    free(res);
    free(users_array);
    return 0;
}

void *connection_handler(void *threadid){
	while(1){
		
		int sockfd,n;
		int* threadnum = (int*)threadid;
	    struct sockaddr_in serv_addr;
	    
	    char buffer[512];
	    
	    /* create socket, get sockfd handle */
	    
	    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd < 0) 
	        error("ERROR opening socket client");

	    /* fill in server address in sockaddr_in datastructure */

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
		
		if(strcmp("random",mode) == 0){
			sprintf(buffer, "get foo%d.txt", r);		// random file number for random mode
		}
		else{
			sprintf(buffer, "get foo%d.txt", r1);		// fixed file for fixed mode
		}
		printf("%s\n",buffer);
	   
	    /* send user message to server */

	    n = write(sockfd,buffer,strlen(buffer));
	    if (n < 0) 
	         error("ERROR writing to socket");
	    
	    users_array[*threadnum].total_requests++;
	    struct timeval req_start, req_end;
	    gettimeofday(&req_start, NULL);

	    bzero(buffer,512);
	    
	    /* read reply from server */
	    
	    while(1){
		    bzero(buffer,512);
		    n = read(sockfd,buffer,511);
		    printf("%d\n", n);
		    if(n==0){
		    	// printf("end of file\n");
		    
		    	/* collecting time stamp for complete file received */
		    	gettimeofday(&req_end, NULL);
		    	float temp_time= (req_end.tv_sec * 1000000 + req_end.tv_usec)- (req_start.tv_sec * 1000000 + req_start.tv_usec);
		    	users_array[*threadnum].response_time = ((users_array[*threadnum].response_time * (users_array[*threadnum].total_completed)) + temp_time)/ ((users_array[*threadnum].total_completed) +1);
		    	users_array[*threadnum].total_completed++;
		    	break;	
		    }
		    if (n < 0) {
		         error("ERROR reading from socket");
		         break;
		     }
		     
	    }
	    
	    gettimeofday(&end, NULL);
	    global_time = (end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec);
	     
	     //checking if time is over 
	     if(global_time > (tot_time * 1000000)){
	     	close(sockfd);
	     	break; 
	     }
	     else{
	     	// putting the thread to sleep
	     	sleep(sleep_time);
	     	gettimeofday(&end, NULL);
	     	global_time = (end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec);
	     	if(global_time > (tot_time * 1000000)){
	    		 close(sockfd);
	     		break; 
	     	}	
	     	close(sockfd);
	     }
	}

	return 0;
}
