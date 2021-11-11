/* lqueue.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Thu Nov 11 10:32:21 2021 (-0500)
 * Version: 1
 * 
 * Description: Locked queue module
 * 
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
 
typedef struct locked_queue {
	queue_t* queue;
	pthread_mutex_t* mutex;
} lqueue_t;

lqueue_t* lqopen() {
	pthread_mutex_t *m;
	lqueue_t *lqp;

	lqp = (lqueue_t*)malloc(sizeof(lqueue_t));

	lqp->queue = qopen();
	
	m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(m, NULL);

	lqp->mutex = m;

	return lqp;
	
}

void lqclose(lqueue_t *lqp) {

	qclose(lqp->queue);
	pthread_mutex_destroy(lqp->mutex);
	free(lqp->mutex);
	free(lqp);
	
}

int32_t lqput(lqueue_t *lqp, void *elementp) {
	pthread_mutex_lock(lqp->mutex);

	int32_t output = qput(lqp->queue, elementp);

	pthread_mutex_unlock(lqp->mutex);
	return output;
}

void* lqget(lqueue_t *lqp) {
	pthread_mutex_lock(lqp->mutex);

	void* result = qget(lqp->queue);

	pthread_mutex_unlock(lqp->mutex);
	return result;
}

void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {
	pthread_mutex_lock(lqp->mutex);

	qapply(lqp->queue, fn);

	pthread_mutex_unlock(lqp->mutex);
}

void* lqsearch(lqueue_t *lqp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp) {
	pthread_mutex_lock(lqp->mutex);
	
	void* search_result = qsearch(lqp->queue, searchfn, skeyp);

	pthread_mutex_unlock(lqp->mutex);
	return search_result;
}
