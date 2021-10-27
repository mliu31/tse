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
#include <hash.h>
#include <string.h>

int sum = 0;
int sum_freq = 0;
int page_id = 1;

typedef struct queue_element_type {

	int docId;
	int freq;

} q_el; 

typedef struct index_t {

	char *word;
	queue_t *documents;
	int freq;

} id_t;

q_el* makeDocument(int id) {

	q_el* document = (q_el*)calloc(1, sizeof(q_el));

	document->docId = id;
	document->freq = 1;

	return document;

}

id_t* makeindexentry(char *word) {

	id_t* indexentry = malloc(sizeof(id_t));
	q_el* document = makeDocument(page_id);
	
	indexentry->word = word;
	indexentry->freq = 1;
	indexentry->documents = qopen();
	qput(indexentry->documents, document);

	return indexentry;

}

void incrementDocumentFreq(q_el *document) {

	document->freq = (document->freq) + 1;

}

void incrementindexentry(id_t *indexentry) {

	indexentry->freq = (indexentry->freq) + 1;

}

void printindexentry(void *indexentry) {

	id_t *index_e = (id_t*)(indexentry);
	printf("word: %s, freq: %d\n", index_e->word, index_e->freq);

}

void printDocument(void *document) {

	q_el *document_el = (q_el*)(document);
	printf("document id: %d, freq: %d\n", document_el->docId, document_el->freq);

}

void freeindexentry(void *indexentry) {

	id_t *index_e = (id_t*)(indexentry);
	free(index_e->word);
	free(index_e);

}

void freeDocument(void *document) {

	q_el *document_el = (q_el*)(document);
	free(document_el);

}

void sumofindexentries(void *indexentry) {

	id_t *index_e = (id_t*)(indexentry);
	int *sum_p = &sum;
	
	*sum_p = *sum_p + index_e->freq;

}

void sumDocumentFrequencies(void *document) {

	q_el *document_el = (q_el*)(document);
	int *sum_freq_p = &sum_freq;
	
	*sum_freq_p = *sum_freq_p + document_el->freq;

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
	id_t *ep = (id_t*)elementp;
	char *sp = (char*)searchkeyp;

	if(strcmp(ep->word, sp) == 0) {
		return true;
	}
	return false;
}

bool search_queue(void* elementp, const void* searchkeyp) {
	q_el *ep = (q_el*)elementp;
	int *sp = (int*)searchkeyp;

	if(ep->docId == sp) {
		return true;
	}
	return false;
}

int main(void) {

	webpage_t *loadedpage;
	hashtable_t *hashtable;
	id_t *indexentry;
	q_el *document;
	char *word;
	int pos = 0;

	loadedpage = pageload(id, "../pages/");
	hashtable = hopen(20);
	/*
	webpage_getNextWord(loadedpage, pos, &word);

	indexentry = hsearch(hashtable, search, word, strlen(word));

	if(indexentry == NULL)
		printf("Correct\n");

	indexentry = makeindexentry(word);

	hput(hashtable, indexentry, word, strlen(word));

	printf("**********\n");
	happly(hashtable, printindexentry);
	//printf("word: %s\n", word);
	printf("**********\n");
	
	indexentry = hsearch(hashtable, search, word, strlen(word));

	if(indexentry == NULL)
		printf("Incorrect\n");
	*/	
	
	while(true) {

		pos = webpage_getNextWord(loadedpage, pos, &word);

		if(pos == -1) {
			break;
		}
		
		if(NormalizeWord(&word) == 0) {
			
			indexentry = hsearch(hashtable, search, word, strlen(word));
			
			if(indexentry == NULL) {
				indexentry = makeindexentry(word);
				//printf("************************\n");
				//printindexentry(indexentry);
				//printf("************************\n");
				hput(hashtable, indexentry, word, strlen(word));
			} else {
				//printf("************************\n");
				// in entry, search its queue of docs. If this id found, increase freq, else put another document element in the queue

				document = qsearch(indexentry->documents, search_queue, page_id);

				if(document == NULL) { // word found in new document
					document = makeDocument(page_id);
					qput(indexentry->documents, document);
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
