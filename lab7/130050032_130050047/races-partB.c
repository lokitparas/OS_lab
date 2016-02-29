#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
int count = 0;
int K;
int locked = 0;
struct timeval start, end;	

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void *connection_handler(void *socket_desc);
int main(int argc, char *argv[])
{
    int i,N,global_time;
    void *res;
    count = 0;
    N = atoi(argv[1]);
    K = atoi(argv[2]);

  	gettimeofday(&start, NULL);  // start time for the experiment noted


	/* creating threads */
   	pthread_t sniffer_thread[N];
    for (i=0; i<N; i++) {
        if( pthread_create( &sniffer_thread[i] , NULL ,  connection_handler , (void*) &i) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }
    
    /* joining threads */
    for (i = 0; i < N; ++i)
    {	if(pthread_join(sniffer_thread[i],&res) != 0){
    		perror("Error Joining\n");
    	}
    }
    gettimeofday(&end, NULL);
	global_time = (end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec);
	
	printf("Count -> %d\n",count);
	printf("Time taken -> %d\n", global_time);
    /* Calculating final avg statistics */
    pthread_exit(NULL);
    free(res);
    return 0;
}

void *connection_handler(void *threadid){
	int* threadnum = (int*)threadid;
	int r=0;
	while(r<K){
		while(locked); //do nothing; busy wait
		locked = 1; //set lock
		count = count+1; //increment
		locked = 0; //release lock
		
		r++;     
	}    
	return 0;
}
