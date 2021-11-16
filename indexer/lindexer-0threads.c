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


int main(int argc, char *argv[]) {
	webpage_t *loadedpage;
	lht_t *hashtable;
	wqe_t *document;
	idxe_t *indexentry;
	char *word;
	int pos;
	int id = 1;
	// char *endid;
	char *pagedirectory;
	char *indexnm;

	pagedirectory = argv[1];
	indexnm = argv[2];

	hashtable = lhopen(20);

	loadedpage = pageload(id, pagedirectory);
	if(loadedpage == NULL) {
			printf("failed to load first page"); 
			return -1;
		}
	// printf("id: 1 -- loaded first pg\n");
	
	while(true) {  // loop thru all docs 

		pos = 0;
		printf("id: %d\n", id);	
	
		while(true) {  // loop thru all words in a doc
			pos = webpage_getNextWord(loadedpage, pos, &word);
			
			if(pos == -1) {
				break;
			}

			if(NormalizeWord(&word) == 0) {
				//printf("    pos: %d; word: %s\n", pos, word); 
				indexentry = lhsearch(hashtable, search, word, strlen(word));
				if(indexentry == NULL) {
					indexentry = makeindexentry(word, id);
					if(lhput(hashtable, indexentry, word, strlen(word)) != 0) 
						printf("failed to put into locked hash");
				} else {
					document = qsearch(indexentry->word_queue_p, search_queue, &id);

					if(document == NULL) { // word found in new document
						document = makeQueueEntry(id);
						qput(indexentry->word_queue_p, document);
					} else {
						incrementDocumentFreq(document);
					}
					
					free(word);
				}
			}
		}

		webpage_delete(loadedpage);
		id++;

		loadedpage = pageload(id, pagedirectory);
		//printf("id: %d\n", id); 
		
		if(loadedpage == NULL) {
			printf("didn't load in page %d\n", id); 
			break;
		}	
	}
	printf("finished all docs"); 

	lindexsave(hashtable, indexnm);
	
	lhapply(hashtable, freeindexentry);
	// free all documents documents from queues
	lhclose(hashtable);
	//free queues as well?
		
	return 0;	
}
