#include <pthread.h>
#include <stdio.h>
#define NTHREADS 100

long int counter = 0;
	pthread_mutex_t lock;
//	The thread starter function
void *myThread()
{
	for(int i=0; i<1000; i++){
		pthread_mutex_lock(&lock);
		counter++;
		pthread_mutex_unlock(&lock);
		
	}
}

int main()
{
	// Create space for pthread variables
	pthread_t tid[NTHREADS];

	if(pthread_mutex_init(&lock,NULL) != 0) {
		printf("Mutex init failed\n");
		return 0;
	}
	for(int i=0; i<NTHREADS; i++)
	{
		//	Create a thread with default attributes and no arguments
		pthread_create(&tid[i], NULL, myThread, NULL);
	}

	for(int i=0; i<NTHREADS; i++)
	{
		//	Wait for all threads to finish
		pthread_join(tid[i], NULL);
	}
	pthread_mutex_destroy(&lock);
	printf("Counter: %ld \n", counter);

	return 0;
}
