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
#include <queue.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <string.h>


int sum = 0;
int sum_freq = 0;
int page_id = 1;

typedef struct queue_element_type {

	int docId;
	int freq;

} q_el; 


typedef struct index_entry_t {
	char *word;
	int freq; // NEED TO REMOVE 
	queue_t *word_queue_p; 
} idxe_t;

typedef struct word_queue_entry_t {
	int doc_id;
	int doc_word_freq; 
} wqe_t; 

wqe_t* makeQueueEntry(int id) {

	wqe_t* queue_entry = (wqe_t*)calloc(1, sizeof(wqe_t));

	queue_entry->doc_id = id;
	queue_entry->doc_word_freq = 1;

	return queue_entry;

}

idxe_t* makeindexentry(char *word) {
	idxe_t* indexentry = malloc(sizeof(idxe_t));
	wqe_t* queue_entry = makeQueueEntry(page_id);

	indexentry->word = word;
	indexentry->freq = 1;
	indexentry->word_queue_p = qopen();
	qput(indexentry->word_queue_p, queue_entry);

	return indexentry; 
}

void incrementDocumentFreq(wqe_t *document) {
	document->doc_word_freq = (document->doc_word_freq) + 1;
}

void incrementindexentry(idxe_t *indexentry) {
	indexentry->freq = (indexentry->freq) + 1;
}


void printindexentry(void *indexentry) {
	idxe_t *index_e = (idxe_t*)(indexentry);
	printf("%s: %d\n", index_e->word, index_e->freq); 
}

void printDocument(void *document) {

	wqe_t *document_el = (wqe_t*)(document);
	printf("document id: %d, freq: %d\n", document_el->doc_id, document_el->doc_word_freq);

}

void freeindexentry(void *indexentry) {
	idxe_t *index_e = (idxe_t*)(indexentry);
	free(index_e->word);
	free(index_e);
}

void freeDocument(void *document) {

	wqe_t *document_el = (wqe_t*)(document);
	free(document_el);

}

void sumofindexentries(void *indexentry) {
	idxe_t *index_e = (idxe_t*)(indexentry);
	int *sum_p = &sum;
	
	*sum_p = *sum_p + index_e->freq;
}

void sumDocumentFrequencies(void *document) {

	wqe_t *document_el = (wqe_t*)(document);
	int *sum_freq_p = &sum_freq;
	
	*sum_freq_p = *sum_freq_p + document_el->doc_word_freq;

}

int NormalizeWord(char **wordptr) {
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

	if(i < 4) {
		free(word);
		return -1;
	}
	return 0;
}


bool search(void* elementp, const void* searchkeyp) {
	idxe_t *ep = (idxe_t*)elementp;
	char *sp = (char*)searchkeyp;

	if(strcmp(ep->word, sp) == 0) {
		return true;
	}
	return false;
}

bool search_queue(void* elementp, const void* searchkeyp) {
	wqe_t *ep = (wqe_t*)elementp;
	int *sp = (int*)searchkeyp;

	if(ep->doc_word_freq == sp) {
		return true;
	}
	return false;
}

int main(void) {
	webpage_t *loadedpage;
	hashtable_t *hashtable;
	wqe_t *document;
	idxe_t *indexentry;
	char *word;
	int pos = 0;

	loadedpage = pageload(id, "../pages/");
	hashtable = hopen(20);
	
	while(true) {
		pos = webpage_getNextWord(loadedpage, pos, &word);

		if(pos == -1) {
			break;
		}
		
		if(NormalizeWord(&word) == 0) {
			indexentry = hsearch(hashtable, search, word, strlen(word));
			if(indexentry == NULL) {
				indexentry = makeindexentry(word);
				hput(hashtable, indexentry, word, strlen(word));
			} else {
				//printf("************************\n");
				// in entry, search its queue of docs. If this id found, increase freq, else put another document element in the queue

				document = qsearch(indexentry->word_queue_p, search_queue, page_id);

				if(document == NULL) { // word found in new document
					document = makeDocument(page_id);
					qput(indexentry->word_queue_p, document);
				} else {
					incrementDocumentFreq(document);
				}
					
				incrementindexentry(indexentry);
				free(word);
			}
		}
	}
	
	happly(hashtable, printindexentry);
	// print each document in all queues
	happly(hashtable, sumofindexentries);
	// sum all frequencies from all queues
	printf("sum: %d\n", sum);
	
	happly(hashtable, freeindexentry);
	// free all documents documents from queues
	hclose(hashtable);
	//free queues as well?

	webpage_delete(loadedpage);

	return 0;
}
