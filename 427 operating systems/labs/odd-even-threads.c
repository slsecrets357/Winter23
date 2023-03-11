/*
Consider a randomly initialised array consisting of positive integers.
You need to collect all the even indexed elements on one thread and all the odd indexed elements on a separate thread.
Make sure to go over the indexes in increasing order without skipping any.
Lastly, gather sum of only odd integers in even indexed thread and gather sum of only even integers in the odd indexed thread.

An example:
-----------
Some random array like = 3 1 55 4 5 8 7
Thread 1 will have:
Even indexed elements of the array: 3, 55, 5, 7
	collect sum of only odd integers from this - 3+55+5+7 = 70 is the output

thread 2
Odd indexed elements of the array: 1, 4, 8
	collect sum of only even integers from this - 4+8 = 12 is the output

*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define SIZE 20

pthread_mutex_t lock; // mutex lock that will be used to lock the critical section
pthread_cond_t cond; // condition variable that will be used to signal the other thread
int pos = 0;

struct tracker{
	int * arr;
	int evenSum;
	int oddSum;
};

void * evenWorker(void * arg){ // evenWorker
	/*
		1. Lock the mutex
		2. Check if the index is even
		3. If yes, print the element and add it to the sum if it is odd
		4. Increment the index
		5. Signal the other thread
		6. Unlock the mutex
	*/
	struct tracker * output = arg; // typecast the void pointer to a struct pointer
	
	while(pos < SIZE){
		// in this while loop, the evenWorker thread will go over all the elements of the array
		// and add the odd elements to the sum. 
		// The oddWorker thread will add the even elements to the sum.
		// The two threads will do this in an alternating fashion.
		// basically, after the evenWorker thread has added all the odd elements to the sum, 
		// it will signal the oddWorker thread to add the even elements to the sum and unlock the mutex so that the oddWorker thread can access the critical section.
		// then the oddWorker thread will add the even elements to the sum and signal the evenWorker thread to add the odd elements to the sum and unlock the mutex so that the evenWorker thread can access the critical section.
		pthread_mutex_lock(&lock); // lock the mutex so that the other thread cannot access the critical section
		if(pos%2==0){
			printf("Tid %ld even index %d element: %d\n", pthread_self(), pos, output->arr[pos]);
			if(output->arr[pos]%2 != 0){
				output->evenSum = output->evenSum + output->arr[pos];
			}
			pos++;
			pthread_cond_signal(&cond); // signal the other thread that it can now access the critical section
		}
		else{
			pthread_cond_wait(&cond, &lock); // wait for the other thread to signal
		}
		pthread_mutex_unlock(&lock); // unlock the mutex so that the other thread can access the critical section
	}
	pthread_exit(NULL); // exit the thread. Null is returned as the return value
}

void * oddWorker(void * arg){
	struct tracker * output = arg;
	
	while(pos < SIZE){
		pthread_mutex_lock(&lock);
		if(pos%2!=0){
			printf("Tid %ld odd index %d element: %d\n", pthread_self(), pos, output->arr[pos]);
			if(output->arr[pos]%2 == 0){
				output->oddSum = output->oddSum + output->arr[pos];
			}
			pos++;
			pthread_cond_signal(&cond);
		}
		else{
			pthread_cond_wait(&cond, &lock);
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){

	// int arr[7] = {3, 1, 55, 4, 5, 8, 7};

	int arr[SIZE];
	// initialise an array with random values.
    for (int i=0;i<SIZE;i++){
        arr[i] = 1+ rand() % (30+1-1);
		printf("%d ", arr[i]);
    }
	printf("\n");
	struct tracker *output = malloc(sizeof(struct tracker));
	output->arr = malloc(sizeof(int)*SIZE);
	output->arr = arr;
	output->evenSum=0;
	output->oddSum=0;

	pthread_mutex_init(&lock, NULL); // initialise the mutex lock so that it can be used
	pthread_cond_init(&cond, NULL);
	pthread_t thread[2]; // create two threads - one for evenWorker and one for oddWorker

	pthread_create(&thread[0], NULL, evenWorker, output); // create the evenWorker thread
	pthread_create(&thread[1], NULL, oddWorker, output); // create the oddWorker thread

	void *result[2];

	// pthread_join(thread[0], &result[0]);
	// pthread_join(thread[1], &result[1]);

	pthread_join(thread[0], NULL); // wait for the evenWorker thread to finish
	pthread_join(thread[1], NULL); // wait for the oddWorker thread to finish


	printf("Output from thread 0 (evenWorker) is:%d\n", output->evenSum);
	printf("Output from thread 1 (oddWorker) is:%d\n", output->oddSum);

	pthread_mutex_destroy(&lock); // destroy the mutex lock so that it can be used again
	pthread_cond_destroy(&cond); // destroy the condition variable so that it can be used again
	return 0;
}