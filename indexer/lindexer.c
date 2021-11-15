/* indexer.c --- 
 * 
 * 
 * Author: Agon Hoxha
 * Created: Tue Oct 26 19:04:11 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <pageio.h>
#include <queue.h>
#include <webpage.h>
#include <hash.h>
#include <string.h>
#include "indexio.h"


int sum = 0;

/*
typedef struct index_entry_t {
	char *word;
	queue_t *word_queue_p; 
} idxe_t;


typedef struct word_queue_entry_t {
	int doc_id;
	int doc_word_freq;
} wqe_t;
*/

static wqe_t* makeQueueEntry(int id) {
	wqe_t* queue_entry = (wqe_t*)calloc(1, sizeof(wqe_t));

	queue_entry->doc_id = id;
	queue_entry->doc_word_freq = 1;

	return queue_entry;
}


static idxe_t* makeindexentry(char *word, int id) {
	idxe_t* indexentry = malloc(sizeof(idxe_t));
	wqe_t* queue_entry = makeQueueEntry(id);

	indexentry->word = word;
	indexentry->word_queue_p = qopen();
	qput(indexentry->word_queue_p, queue_entry);

	return indexentry;
}


static void incrementDocumentFreq(wqe_t *document) {
	document->doc_word_freq = (document->doc_word_freq) + 1;
}

/*void incrementindexentry(idxe_t *indexentry) {
	indexentry->freq = (indexentry->freq) + 1;
	}


static void printDocument(void *document) {
	wqe_t *document_el = (wqe_t*)(document);
	printf("id: %d, freq: %d\n", document_el->doc_id, document_el->doc_word_freq);
}


static void printindexentry(void *indexentry) {
	idxe_t *index_e = (idxe_t*)(indexentry);
	printf("**********\n");
	printf("word: %s\n", index_e->word);
	qapply(index_e->word_queue_p, printDocument);
	printf("**********\n");
}
*/

static void freeDocument(void *document) {
	wqe_t *document_el = (wqe_t*)(document);
	free(document_el);
}


static void freeindexentry(void *indexentry) {
	idxe_t *index_e = (idxe_t*)(indexentry);
	
	qapply(index_e->word_queue_p, freeDocument);
	qclose(index_e->word_queue_p);
	
	free(index_e->word);
	free(index_e);
}

/*
static void sumDocumentFrequencies(void *document) {
	wqe_t *document_el = (wqe_t*)(document);
	int *sum_freq_p = &sum;
	
	*sum_freq_p = *sum_freq_p + document_el->doc_word_freq;
}


static void sumofindexentries(void *indexentry) {
	idxe_t *index_e = (idxe_t*)(indexentry);

	qapply(index_e->word_queue_p, sumDocumentFrequencies);
}

*/
static int NormalizeWord(char **wordptr) {
	char *word = *wordptr;
	int i = 0;

	while(word[i] != '\0') {
		if(isalpha(word[i]) == 0) {
			free(word);
			return -1;
		}
		word[i] = ((char)(((int)tolower(word[i]))));
		i++;
	}

	if(i < 3) {
		free(word);
		return -1;
	}
	return 0;
}


static bool search(void* elementp, const void* searchkeyp) {
	idxe_t *ep = (idxe_t*)elementp;
	char *sp = (char*)searchkeyp;

	if(strcmp(ep->word, sp) == 0) {
		return true;
	}
	return false;
}


static bool search_queue(void* elementp, const void* searchkeyp) {
	wqe_t *ep = (wqe_t*)elementp;
	int *sp = (int*)searchkeyp;

	if(ep->doc_id == *sp) {
		return true;
	}
	return false;
}


// reads through documents word-by-word
void* read_document_wbw(webpage_t *loadedpage, char* word, hashtable_t *hashtable, int *id, char* pagedirectory) {
	while(true) {
		int pos = webpage_getNextWord(loadedpage, pos, &word);

		if(pos == -1) 
			break;
		
		if(NormalizeWord(&word) == 0) {
			idxe_t *indexentry = hsearch(hashtable, search, word, strlen(word));
			if(indexentry == NULL) {
				indexentry = makeindexentry(word, *id);
				hput(hashtable, indexentry, word, strlen(word));
			} else {

				// in entry, search its queue of docs
				wqe_t *document = qsearch(indexentry->word_queue_p, search_queue, &id);

				// If this id found, increase freq
				if(document == NULL) { // word found in new document
					document = makeQueueEntry(*id);
					qput(indexentry->word_queue_p, document);
				} else { // else put another document element in the queue
					incrementDocumentFreq(document);
				}
					
				free(word);
			}
		}
	

		webpage_delete(loadedpage);
		id++;

		printf("id: %d\n", *id);
		loadedpage = pageload(*id, pagedirectory);
		if(loadedpage == NULL) {
			break;
		}
	}
	return 0; 
}

int main(int argc, char *argv[]) {
	webpage_t *loadedpage;
	hashtable_t *hashtable;
	//wqe_t *document;
	//idxe_t *indexentry;
	char *word;
	// int pos;
	int* id; 
	// char *endid;
	char *pagedirectory;
	char *indexnm;
	int num_threads = 2;
	pthread_t *threads[num_threads]; 

	
	pagedirectory = argv[1];
	indexnm = argv[2];
	*id = 1; 
	
	hashtable = hopen(20);

	loadedpage = pageload(*id, pagedirectory);
	printf("loaded first page\n");
	
	while(true) {

		int pos = 0;

		for(int tid=0; tid<num_threads; tid++) { 
			
			if(pthread_create(threads[tid], NULL, read_document_wbw, loadedpage, word, hashtable, id, pagedirectory)) {
				printf("thread %d failed to create\n", tid);
				exit(EXIT_FAILURE);
			}
		}
	}

	for(int tid=0; tid<num_threads; tid++) {
		if(pthread_join(&tid, NULL) != 0)
			exit(EXIT_FAILURE);
	}
	
	indexsave(hashtable, indexnm);
	 
		
	//happly(hashtable, printindexentry);
	// print each document in all queues
	//happly(hashtable, sumofindexentries);
	// sum all frequencies from all queues
	//printf("sum: %d\n", sum);
	

	happly(hashtable, freeindexentry);
	// free all documents documents from queues
	hclose(hashtable);
	//free queues as well?
		
	return 0;	
}
