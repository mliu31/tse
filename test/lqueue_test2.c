/* lqueue_test2.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Thu Nov 11 11:21:22 2021 (-0500)
 * Version: 1
 * 
 * Description: Testing lqueue -- uniprocessor (one thread) 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
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


void* modifyQueue(void *lq) {
	lqueue_t* lqueue = (lqueue_t*) lq;  

	return lqueue; 
}


int main() {
	pthread_t tid1; // tid2;
	lqueue_t* lqp;

	if(pthread_create(&tid1, NULL, modifyQueue, NULL) != 0) {
		printf("thread 1 failed to create"); 
		exit(EXIT_FAILURE);
	}
	/*
	if(pthread_create(&tid2, NULL, modifyQueue, NULL) != 0) {
		printf("thread 2 failed to create"); 
		exit(EXIT_FAILURE);
	}
	*/

	printf("thread creations finished\n");

	if(pthread_join(tid1,NULL) != 0)
	  exit(EXIT_FAILURE);
	/*
	if(pthread_join(tid2,NULL) != 0)
		exit(EXIT_FAILURE);
	*/
		
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

	lqclose(lqp);
	free(x);
	
	exit(EXIT_SUCCESS);
}
