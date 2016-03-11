#include<stdio.h>
#include<stdlib.h>

typedef struct QueueNode{
	int value;
	struct QueueNode *node;
}QueueNode;

typedef struct UnboundQueue
{
        int size;
        int max_size;
        struct QueueNode* front;
        struct QueueNode* rear;
}UnboundQueue;



UnboundQueue * U_createQueue()
{
        /* Create a Queue */
		UnboundQueue* Q;
        Q = (UnboundQueue *)malloc(sizeof(UnboundQueue));
        // Q = (Queue *)malloc(sizeof(Queue));
        /* Initialise its properties */
        // Q->elements = (int *)malloc(sizeof(int)*maxElements);
        // bzero(Q->elements,sizeof(int)*maxElements);
        Q->size = 0;
        Q->max_size =-1;
        Q->front = NULL;
        Q->rear = NULL;
        return Q;
}


UnboundQueue * U_createQueue1(int max)
{
        /* Create a Queue */
		UnboundQueue* Q;
        Q = (UnboundQueue *)malloc(sizeof(UnboundQueue));
        // Q = (Queue *)malloc(sizeof(Queue));
        /* Initialise its properties */
        // Q->elements = (int *)malloc(sizeof(int)*maxElements);
        // bzero(Q->elements,sizeof(int)*maxElements);
        Q->size = 0;
        Q->max_size = max;
        Q->front = NULL;
        Q->rear = NULL;
        return Q;
}

void U_Enqueue(UnboundQueue* Q,int n){
	if(Q->max_size!= -1){
		if(Q->size == Q->max_size){
			printf("Queue is full\n");
			return;
		}
	}
	QueueNode* temp;
	temp = (QueueNode *)malloc(sizeof(QueueNode));
    temp->value = n;
    temp->node = NULL;
    if(Q->size ==0){
    	Q->front = temp;
    }
    if(Q->size > 0){Q->rear->node = temp;}
	Q->rear = temp;
	Q->size +=1;
}

void U_Dequeue(UnboundQueue* Q){
	if(Q->size == 0){printf("Queue is empty\n"); return;}
	if(Q->size == 1){
		Q->front = NULL;
		Q->rear = NULL;
		Q->size =0;
		return;
	}
	QueueNode* temp = Q->front->node;
	free(Q->front);
	Q->front = temp;
	Q->size -= 1;
}

int U_front(UnboundQueue *Q)
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                exit(0);
        }
        /* Return the element which is at the front*/
        return Q->front->value;
}

void printQ(UnboundQueue* Q){
	int i;
	QueueNode* temp = Q->front;
	if(Q->size == 0){
		printf("Queue is empty\n");
	}
	for(i=0;i<Q->size;i++){
		printf("%d ",temp->value);
		temp= temp->node;
	}
	printf("\n");
}

int sizeofqueue(UnboundQueue *Q){
    return Q->size;   
}


int queueisfull(UnboundQueue *Q){
    if(Q->size == Q->max_size)
        {
                printf("Queue is Full\n");
                return 1;
        }
        return 0;
}