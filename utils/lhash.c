/*
 * hash.h -- A generic hash table implementation, allowing arbitrary
 * key structures.
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "lhash.h"
#include "hash.h"



/* hopen -- opens a hash table with initial size hsize */
lht_t* lhopen(uint32_t hsize) {
	pthread_mutex_t *m;
	lht_t *lht;

	lht = (lht_t*)malloc(sizeof(lht_t));
	lht->hashtable = hopen(hsize);
	m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(m, NULL);

	lht->mutex = m;

	return lht; 
}

/* hclose -- closes a hash table */
void lhclose(lht_t *lhtp) {
	hclose(lhtp->hashtable);
	pthread_mutex_destroy(lhtp->mutex);
	free(lhtp->mutex);
	free(lhtp); 
}

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lht_t *lhtp, void *ep, const char *key, int keylen) {
	pthread_mutex_lock(lhtp->mutex);

	int32_t output = hput(lhtp->hashtable, ep, key, keylen); 

	pthread_mutex_unlock(lhtp->mutex);
	return output; 
}


/* happly -- applies a function to every entry in hash table */
void lhapply(lht_t *lhtp, void (*fn)(void* ep)) {
	pthread_mutex_lock(lhtp->mutex);
	
	happly(lhtp->hashtable, fn); 
	
	pthread_mutex_unlock(lhtp->mutex);
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lht_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
							 int32_t keylen) {
	pthread_mutex_lock(lhtp->mutex);

	void* sought_elem = hsearch(lhtp->hashtable, searchfn, key, keylen); 
	
	pthread_mutex_unlock(lhtp->mutex);

	return sought_elem; 
}

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lht_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
							 int32_t keylen) {
	pthread_mutex_lock(lhtp->mutex);

	void* removed_elem = hremove(lhtp->hashtable, searchfn, key, keylen); 
	
	pthread_mutex_unlock(lhtp->mutex);

	return removed_elem; 
}

