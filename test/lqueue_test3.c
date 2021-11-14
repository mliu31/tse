/* lqueue_test3.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Thu Nov 11 11:21:22 2021 (-0500)
 * Version: 1
 * 
 * Description: Testing lqueue -- multiprocessor (2 threads) 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "lqueue.h"


void printe(void* elem) {
  int* element = (int*)elem;

	printf("%d-->", *element);
}


bool searchfn(void* elem, const void* key) {
	int *element = (int*)elem;
	int *actual_key = (int*)key;

	if(*element == *actual_key)
		return true;
	return false;
}


void* threadProcessPut(void* locked_queue) {
	
	lqueue_t *lq = (lqueue_t*) locked_queue;
	/*										 
	if (lqput(lq, 3) != 0)
		return -1; 
	return 0; 
	*/
	pthread_mutex_lock(lq->mutex);
	printf("entered threada1 func\n"); 
	sleep(5);
	printf("exited threada1 func\n"); 
	pthread_mutex_unlock(lq->mutex);

	return 0; 
}

void* threadProcessGet(void* locked_queue) {
	lqueue_t *lq = (lqueue_t*) locked_queue;
	// lqget(lq);

	pthread_mutex_lock(lq->mutex);
	printf("entered thread2 func\n"); 
	sleep(2);
	printf("exited thread2 func\n"); 
	pthread_mutex_unlock(lq->mutex); 

	return 0; 
}


int main() {
	pthread_t tid1, tid2;
	lqueue_t* lqp = lqopen();
	
	if(pthread_create(&tid2, NULL, threadProcessGet, lqp) != 0) {
		printf("thread 2 failed to create"); 
		exit(EXIT_FAILURE);
	}
	if(pthread_create(&tid1, NULL, threadProcessPut, lqp) != 0) {
		printf("thread 1 failed to create"); 
		exit(EXIT_FAILURE);
	}
	
	printf("Creations finished\n");

	if(pthread_join(tid1,NULL) != 0)
	  exit(EXIT_FAILURE);
	if(pthread_join(tid2,NULL) != 0)
		exit(EXIT_FAILURE);
	
	/* 
	lqp = lqopen();

	int *x = (int*)malloc(sizeof(int));
	*x = 100;

	lqput(lqp, x);

	printf("locked queue: "); 
	lqapply(lqp, printe);

	int *y = lqget(lqp);
	printf("\nremoved from locked queue: %d\n", *y);
	
	if(*y != 100) {
		printf("qget not working\n");
		exit(EXIT_FAILURE);
	}
		
	if(lqsearch(lqp, searchfn, &y) != NULL) {
		printf("lqsearch not working\n");
		exit(EXIT_FAILURE);
	}

	*/
	
	lqclose(lqp);
	//	free(x);
	
	exit(EXIT_SUCCESS);
}
