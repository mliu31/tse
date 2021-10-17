/* hashfile.c --- 
 * 
 * 
 * Author: Agon Hoxha
 * Created: Sun Oct 10 17:45:07 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"
#include "hash.h"

#define get16bits(d) (*((const uint16_t *) (d)))

typedef struct real_hashtable_t {
	queue_t** table; 
	uint32_t len;
} rht_t;

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
	uint32_t hash = len, tmp;
	int rem;

	if (len <= 0 || data == NULL)
		return 0;
	rem = len & 3;
	len >>= 2;
	/* Main loop */
	for (;len > 0; len--) {
		hash  += get16bits (data);
		tmp    = (get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (uint16_t);
		hash  += hash >> 11;
	}
	/* Handle end cases */
	switch (rem) {
	case 3: hash += get16bits (data);
		hash ^= hash << 16;
		hash ^= data[sizeof (uint16_t)] << 18;
		hash += hash >> 11;
		break;
	case 2: hash += get16bits (data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}
	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;
	return hash % tablesize;
}

hashtable_t *hopen(uint32_t hsize) {
	rht_t *hp;
	
	if (!(hp = (rht_t*)calloc(1, sizeof(rht_t)))) {
		printf("[Error: calloc failed for hashtable]\n");
		return NULL;
	}

	if (!(hp->table = (queue_t**)calloc(hsize, sizeof(queue_t*)))) {
		printf("[Error: calloc failed for table]\n");
		return NULL;
	}

	hp->len = hsize;
	
	for(int i = 0; i < hp->len; i++) {
	  hp->table[i] = qopen();
	}

	return (hashtable_t*)hp; 	
}


void hclose(hashtable_t *htp) {
	rht_t *hp;

	hp = (rht_t*)htp;

	for(int i = 0; i < hp->len; i++) {
	  qclose(hp->table[i]);
	}

	free(hp->table);
	free(hp);
}


int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {

	rht_t *hp;
	uint32_t hashnumber;

	hp = (rht_t*)htp;
	hashnumber = SuperFastHash(key, keylen, hp->len);

	return qput(hp->table[hashnumber], ep);
}

void happly(hashtable_t *htp, void (*fn)(void* ep)) {
	rht_t *hp;

	hp = (rht_t*)htp;

	for(int i = 0; i < hp->len; i++) {
	  qapply(hp->table[i], fn);
	}
}


void *hsearch(hashtable_t *htp,
							bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key,
							int32_t keylen) {
	rht_t *hp;
	uint32_t hashnumber;

	hp = (rht_t*)htp;
	hashnumber = SuperFastHash(key, keylen, hp->len);

	return qsearch(hp->table[hashnumber], searchfn, key);
}


void *hremove(hashtable_t *htp,
							bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key,
							int32_t keylen) {
	rht_t *hp;
	uint32_t hashnumber;

	hp = (rht_t*)htp;
	hashnumber = SuperFastHash(key, keylen, hp->len);

	return qremove(hp->table[hashnumber], searchfn, key);
}
