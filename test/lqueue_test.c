/* lqueue_test.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Thu Nov 11 11:21:22 2021 (-0500)
 * Version: 1
 * 
 * Description: Testing lqueue -- no threads
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


int main() {
	//pthread_t tid1, tid2;
	lqueue_t* lqp;

	/*if(pthread_create() != 0)
		exit(EXIT_FAILURE);
	if(pthread_create() != 0)
		exit(EXIT_FAILURE);


	printf("Creations finished\n");

	if(pthread_join(tid1,NULL) != 0)
		//exit(EXIT_FAILURE);
	if(pthread_join(tid2,NULL) != 0)
		 //exit(EXIT_FAILURE);
	*/
	lqp = lqopen();

	int *x = (int*)malloc(sizeof(int));
	*x = 100;

	lqput(lqp, x);

	printf("locked queue: "); 
	lqapply(lqp, printe);

	int *y = lqget(lqp);
	printf("\nremoved from locked queue: %d\n", *y);
	
	if(*y != 100)
		printf("qget not working\n");

	if(lqsearch(lqp, searchfn, &y) != NULL)
		printf("lqsearch not working\n");

	lqclose(lqp);
	free(x);
	
	exit(EXIT_SUCCESS);
}
