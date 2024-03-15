/*
hw5.c
gcc hw5.c -o hw5 -lpthread
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define TRUE 1
#define BUFFER_SIZE 5
#define LOOPS 10
/* the semaphores */
sem_t full, empty, mutex;
/* the buffer */
int buffer[BUFFER_SIZE];
/* buffer counter */
int in, out;
pthread_t tid; //Thread ID
pthread_attr_t attr; //Set of thread attributes
void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */
void initializeData() {
	/* Create the mutex lock */
	sem_init(&mutex, 0, 1);
	/* Create the full semaphore and initialize to 0 */
	sem_init(&full, 0, 0);
	/* Create the empty semaphore and initialize to BUFFER_SIZE */
	sem_init(&empty, 0, BUFFER_SIZE);
	/* Get the default attributes */
	pthread_attr_init(&attr);
	/* init buffer */
	in = 0;
	out = 0;
}
/* Producer Thread */
void *producer(void *param) {
	int item;
	int i = 0;
	while(i < LOOPS)
	{ /* generate a random number */
		item = rand()%1000;
		//ADD YOUR CODE
		sem_wait(&empty);	//Request an empty buffer.
		sem_wait(&mutex);	//Request the lock.
		buffer[in] = item;	//Write item to buffer.
		in = (in+1)%BUFFER_SIZE;//Increment buffer.
		//Inform user.
		printf("Producer [%lu] produced %d.\n", pthread_self(), item);
		i++;			//Increment the loop counter.
		sem_post(&mutex);	//Release the lock.
		sem_post(&full);	//Increment number of full buffers.
	}
}
/* Consumer Thread */
void *consumer(void *param) {
	int item;
	int i = 0;
	while(i < LOOPS)
	{
		// ADD YOUR CODE
		sem_wait(&full);
		sem_wait(&mutex);
		item = buffer[out];
		out = (out+1)%BUFFER_SIZE;
		//Inform user.
		printf("Consumer [%lu] consumed %d.\n", pthread_self(), item);
		sem_post(&mutex);
		sem_post(&empty);
	}
}
int main(int argc, char *argv[]) {
	/* Loop counter */
	int i;
	/* Verify the correct number of arguments were passed in */
	if(argc != 3) {
		fprintf(stderr, "USAGE:./pc \n");
	}
	int numProd = atoi(argv[1]); /* Number of producer threads */
	int numCons = atoi(argv[2]); /* Number of consumer threads */
	/* Initialize the app */
	initializeData();
	/* Create the producer threads */
	for(i = 0; i < numProd; i++) {
		/* Create the thread */
		pthread_create(&tid,&attr,producer, NULL);
	}
	/* Create the consumer threads */
	for(i = 0; i < numCons; i++) {
		/* Create the thread */
		pthread_create(&tid,&attr,consumer, NULL);
	}
	/* Sleep for the specified amount of time */
	sleep(1);
	/* Exit the program */
	printf("Exit the program\n");
	exit(0);
}
