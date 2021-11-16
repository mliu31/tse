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
#include <pageio.h>
#include <pthread.h>
#include <webpage.h>
#include <queue.h>
#include "lhash.h"
#include <string.h>
#include "indexio.h"

int sum = 0;
char *pagedirectory; 

typedef struct thread_container { 
	lht_t *hashtable; 
	int page_id; 
} thread_t; 


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
void* read_document_wbw(void *threadID) {
	int pos;
	char *word;
	webpage_t *loadedpage;
	idxe_t *indexentry;
	wqe_t *document;
	
	thread_t *tid = (thread_t*)threadID;
	int id = tid->page_id;
	lht_t *hashtable = tid->hashtable;
	

	pthread_mutex_lock(hashtable->mutex);
	loadedpage = pageload(id, pagedirectory);
	pthread_mutex_unlock(hashtable->mutex);
	
	while(true) {

		pos = 0;
		id = tid->page_id;
	
			while(true) {
				pos = webpage_getNextWord(loadedpage, pos, &word);

				if(pos == -1) 
					break;
		
				if(NormalizeWord(&word) == 0) {
					indexentry = lhsearch(hashtable, search, word, strlen(word));
					if(indexentry == NULL) {
						indexentry = makeindexentry(word, id);
						lhput(hashtable, indexentry, word, strlen(word));
					} else {

						// in entry, search its queue of docs
						document = qsearch(indexentry->word_queue_p, search_queue, &id);

						// If this id found, increase freq
						if(document == NULL) { // word found in new document
							document = makeQueueEntry(id);
							qput(indexentry->word_queue_p, document);
						} else { // else put another document element in the queue
							incrementDocumentFreq(document);
						}
						
						free(word);
					}
				}
			}

			printf("id: %d\n", id);
			webpage_delete(loadedpage);
			
			pthread_mutex_lock(hashtable->mutex);
			(tid->page_id)++;			
			loadedpage = pageload(id, pagedirectory);
			pthread_mutex_unlock(hashtable->mutex);

			if(loadedpage == NULL) 
				break;
			
	}
	return 0; 
}


int main(int argc, char *argv[]) {
	char *indexnm;
	
	int num_threads = 1;
	pthread_t threads[num_threads]; 

	if(argc != 3) { 
		printf("usage: lindexer <pagedir> <indexfilenm>\n"); 
		return -1;
	}
	
	indexnm = argv[2];
	pagedirectory = argv[1];

	thread_t *threadID = (thread_t*)malloc(sizeof(thread_t)); 
	threadID->hashtable = lhopen(20);
	threadID->page_id = 1;

	//printf("loaded first page\n");

	for(int i=0; i<num_threads; i++) { 
		// add all words in a doc to indexer

		printf("thread: %d\n", i);

		if(pthread_create(&threads[i], NULL, read_document_wbw, threadID)) {
			printf("thread %d failed to create\n", i);
			exit(EXIT_FAILURE);
		}
	}

	for(int tid=0; tid<num_threads; tid++) {
		if(pthread_join(((threads[tid])), NULL) != 0)
			exit(EXIT_FAILURE); 
	}

	lindexsave(threadID->hashtable, indexnm); 

	lhapply(threadID->hashtable, freeindexentry);
	lhclose(threadID->hashtable);
	free(threadID); 
	return 0;	
}
