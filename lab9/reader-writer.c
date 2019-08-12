#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


struct read_write_lock
{
	// If required, use this strucure to create
	// reader-writer lock related variables
	int readers;
	int writers;
	int waiting_writers;
	pthread_mutex_t lock;
	
}rwlock;

long int data = 0;			//	Shared data variable
pthread_cond_t cond[2];
void InitalizeReadWriteLock(struct read_write_lock * rw)
{
	rw->readers = 0;
	rw->writers = 0;
	rw->waiting_writers = 0;
	if (pthread_mutex_init(&rw->lock,NULL) != 0) {
		printf("Error in intialising the lock\n");
		exit(0);
	}
}


// The pthread based reader lock
void ReaderLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	while(rw->writers > 0 || rw->waiting_writers > 0) pthread_cond_wait(&cond[0],&rw->lock);
	rw->readers++;
	pthread_mutex_unlock (&rw->lock);
}	

// The pthread based reader unlock
void ReaderUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	rw->readers--;
	if(rw->readers==0) {
		pthread_cond_signal(&cond[1]);
		// pthread_cond_signal(&cond[0]);
	}
	pthread_mutex_unlock(&rw->lock);
}

// The pthread based writer lock
void WriterLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	rw->waiting_writers++;
	if (rw->readers > 0 || rw->writers > 0) pthread_cond_wait(&cond[1],&rw->lock);
	rw->writers++;
	rw->waiting_writers--;
	pthread_mutex_unlock(&rw->lock);
}

// The pthread based writer unlock
void WriterUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->lock);
	rw->writers--;


	if(rw->writers == 0) {
		pthread_cond_signal(&cond[1]);
		if(rw->waiting_writers == 0) {pthread_cond_broadcast(&cond[0]);}
	}
	pthread_mutex_unlock(&rw->lock);
}

//	Call this function to delay the execution by 'delay' ms
void delay(int delay)
{
	struct timespec wait;

	if(delay <= 0)
		return;

	wait.tv_sec = delay / 1000;
	wait.tv_nsec = (delay % 1000) * 1000 * 1000;
	nanosleep(&wait, NULL);
}

struct argument_t {
	int t;
	int id;
};

// The pthread reader start function
void *ReaderFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct argument_t *dat = args;
	delay(dat->t);
	ReaderLock(&rwlock);
	printf("Reader %d, data: %ld\n",dat->id,data);
	delay(1);
	ReaderUnlock(&rwlock);
	//	....
	
	//  Get appropriate lock
	//	Display  thread ID and value of the shared data variable
	//
    //  Add a dummy delay of 1 ms before lock release  
	//	....
}

// The pthread writer start function
void *WriterFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct argument_t *dat = args;
	delay(dat->t);
	WriterLock(&rwlock);
	data++;
	// printf("data %ld\n",data);
	printf("Writer %d, data: %ld\n",dat->id,data);
	delay(1);
	WriterUnlock(&rwlock);
	//	....struct argument_t *arg = (struct argument_t *)malloc(sizeof(struct argument_t *));
	//
	//  Get appropriate lock
	//	Increment the shared data variable
	//	Display  thread ID and value of the shared data variable
	//
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	
}

int main(int argc, char *argv[])
{
	pthread_t *threads;

	
	long int reader_number = 0;
	long int writer_number = 0;
	long int thread_number = 0;
	long int total_threads = 0;	
	
	int ind = 0;
	int count = 0;			// Number of 3 tuples in the inputs.	Assume maximum 10 tuples 
	int rws[10];			
	int nthread[10];
	int delay[10];
	// printf("0\n");
	InitalizeReadWriteLock(&rwlock);
	// printf("1\n");
	if (pthread_cond_init(&cond[0],NULL) != 0) {
		printf("Error in intialising the lock\n");
		return 0;
	}


	if (pthread_cond_init(&cond[1],NULL) != 0) {
		printf("Error in intialising the lock\n");
		return 0;
	}
	//	Verifying number of arguments
	if(argc<4 || (argc-1)%3!=0)
	{
		printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");		
		printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
		exit(1);
	}
	
	//	Reading inputs
	for(int i=0; i<(argc-1)/3; i++)
	{
		char rw[2];
		strcpy(rw, argv[(i*3)+1]);

		if(strcmp(rw, "r") == 0 || strcmp(rw, "w") == 0)
		{
			if(strcmp(rw, "r") == 0)
				rws[i] = 1;					// rws[i] = 1, for reader
			else
				rws[i] = 2;					// rws[i] = 2, for writer
			
			nthread[i] = atol(argv[(i*3)+2]);		
			delay[i] = atol(argv[(i*3)+3]);

			count ++;						//	Number of tuples
			total_threads += nthread[i];	//	Total number of threads
		}
		else
		{
			printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");
			printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
			exit(1);
		}
	}
	// printf("1\n");
	threads = (pthread_t *)malloc(total_threads*sizeof(pthread_t));
	// printf("2\n");
	for (int i=0;i<count;i++){
		// printf("count - i\n");
		for (int j=0;j<nthread[i];j++){
			
			
			if(rws[i] == 1) {
				struct argument_t *arg = (struct argument_t *)malloc(sizeof(struct argument_t));
				arg->t = delay[i];
				reader_number++;
				arg->id = reader_number;
				pthread_create(&threads[ind],NULL,ReaderFunction,(void *)arg);
				// free(arg);
			}
			else {
				struct argument_t *arg = (struct argument_t *)malloc(sizeof(struct argument_t));
				arg->t = delay[i];
				writer_number++;
				arg->id = writer_number;
				pthread_create(&threads[ind],NULL,WriterFunction,(void *)arg);
				// free(arg);
			}
			// free(arg);
			ind++;
		}
	}
	//	Create reader/writer threads based on the input and read and write.
	for (int i=0;i<total_threads;i++) {
		pthread_join(threads[i],NULL);
		
	}
	//  Clean up threads on exit
	free(threads);
	// printf("data -  %ld \n",data);	
	exit(0);
}
