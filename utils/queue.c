/* queue.c --- 
 * 
 * 
 * Author: Megan Y. Liu
 * Created: Wed Oct  6 09:08:30 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "queue.h"

typedef struct real_qelem_t {
	struct real_qelem_t *next; 
	void *data;
} rqe_t; 

typedef struct real_queue_t {
	rqe_t *front;
	rqe_t *back;  
} rq_t; 

queue_t* qopen(void) {
  rq_t *qp;

  if (!(qp = (rq_t*)calloc(1, sizeof(rq_t)))) {
    printf("[Error: malloc failed for queue]\n");
    return NULL; 
  }

	qp->front=NULL;
	qp->back=NULL;

  return (queue_t*)qp; 
}

void qclose(queue_t *qp) {
	//queue_t* whatever = qp;
	rq_t *rq;
	rqe_t *e, *prev;

	rq = (rq_t*)(qp);
	e = rq->front;

	while(e != NULL) {
		prev = e;
		e = e->next;

		free(prev);
	}
		
	free(rq);
}
	
int32_t qput(queue_t* qp, void* elementp) {
	rq_t *rq;
	rqe_t *rqe;

	rq = (rq_t*)qp;	
	
	if (!(rqe = (rqe_t*)calloc(1, sizeof(rqe_t)))) {
		printf("[Error: malloc failed for queue element]\n");
		return -1;
	}

	rqe->data = elementp;
	rqe->next = NULL;
	
	if (rq->back==NULL && rq->front==NULL) 
		rq->front = rqe; 
	else 
		rq->back->next = rqe;

	rq->back = rqe; 
	return 0; 
}

void* qget(queue_t *qp) {

	rq_t *rq;

	rq = (rq_t*)qp;

	if (rq->back==NULL && rq->front==NULL) {
		printf("Cannot get() element from empty queue\n");
		return NULL;
	}
	else if(rq->back==rq->front) {
		void *removed_data;

		removed_data = rq->front->data;
		free(rq->front);
		rq->front = NULL;
		rq->back = NULL;

		return removed_data;
	}
	else {
		void *removed_data;
		rqe_t *e;

		e = rq->front;

		removed_data = e->data;
		rq->front = rq->front->next;

		free(e);

		return removed_data;
	}
	
}
void* qsearch(queue_t *qp, bool (*searchfn)(void* elementp, const void* keyp), const void* skeyp) {

	rq_t *rq;
	rqe_t *e;
	
	rq = (rq_t*)qp;
	e = rq->front;
	
	while(e != NULL) {
		if(searchfn(e->data, skeyp) == true) {
			return e->data;
		}
		e = e->next;
	}
	
	return NULL;

}

void qapply(queue_t *qp, void(*fn)(void* elementp)) {

	rqe_t* e;
	rq_t *rq;
	
	rq = (rq_t*)qp;

	if(rq == NULL) {
		printf("No queue\n");
		return;
	}
	
	e = rq->front;

	if(e == NULL){
		printf("Empty queue\n");
		return;
	}
	
	while(e != NULL){
		fn(e->data);
		e = e->next;
	}	
	
}

void* qremove(queue_t *qp, bool (*searchfn)(void* elementp, const void* keyp), const void* skeyp) {

	rq_t *rq;
	rqe_t *e, *prev;
	void *removed_data;

	if(qp == NULL || skeyp == NULL) {
		printf("[Error: a parameter is null]\n"); 
		return NULL; 
	}
	
	rq = (rq_t*)qp;
	e = rq->front;
	prev = e;
	
	while(e != NULL) {
		if(searchfn(e->data, skeyp) == true) { // match to remove
			if(e == rq->front && e == rq->back) { // single elem q
				removed_data = e->data;
				free(e);
				rq->front = NULL;
				rq->back = NULL;
				return removed_data;
			}
			if(e == rq->front) { // front of q
				removed_data = e->data;
				rq->front = e->next;
				free(e);
				return removed_data; 
			} if(e == rq->back) { // back of q
				removed_data = e->data; 
				rq->back = prev;
				free(e);
				return removed_data;
			}
			if(e == rq->back) {
				removed_data = e->data;
				rq->back = prev;
				free(e);
				return removed_data;
			}
			removed_data = e->data; 
			prev->next = e->next;
			free(e); 
			return removed_data;
		}
		prev = e;
		e = e->next;
	}
	
	return NULL;
	
}

void qconcat(queue_t *q1p, queue_t *q2p) {

	rq_t *rq1, *rq2;

	rq1 = (rq_t*)q1p;
	rq2 = (rq_t*)q2p;

	if(rq1->front != NULL) {
		if(rq2->front != NULL) {
			rq1->back->next = rq2->front;
			rq1->back = rq2->back;
		}
	} else {
		if(rq2->front != NULL) {
			rq1->front = rq2->front;
			rq1->back = rq2->back;
		}
	}
	
	/*
	if(rq1->front != NULL) {
		rq1->back->next = rq2->front;
		rq1->back = rq2->back;
	} else {
		rq1->front = rq2->front;
		rq1->back = rq2->back;
		}*/
	
	free(rq2);

}
