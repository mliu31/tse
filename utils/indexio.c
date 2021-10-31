/* indexio.c --- 
 * 
 * 
 * Author: Megan Y. Liu
 * Created: Fri Oct 29 10:38:09 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include "queue.h"
#include "hash.h"
#include "pageio.h"
#include "indexio.h"

FILE *file;

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

static void printDocument(void *doc) {
	wqe_t *document = (wqe_t*) doc; 

	fprintf(file, "%d %d ", document->doc_id, document->doc_word_freq); 
}

static void printindexentry(void *index_e) {
	idxe_t *indexentry = (idxe_t*) index_e;

	fprintf(file, "%s ", indexentry->word); 
	qapply(indexentry->word_queue_p, printDocument); 
	fprintf(file, "\n");
}

void indexsave(hashtable_t *index, char *filepath) {
	file = fopen(filepath, "w+");
	
	happly(index, printindexentry); 
	
	fclose(file);
}


hashtable_t* indexload(char *filepath) {
	file = fopen(filepath, "r");
	char word[30];
	int doc, doc_freq;

	char* line; 
	char tmp1, tmp2; 

	// use fgets to loop thru each line
	// for each returned str from fgets, use strtok and use " " as delimiter  -- condition of strtok != NULL

	fgets(line, 100, file); 
	word = strok(line, " ");
	while(word!=NULL) {
		// do something with word
		word = strok(line, " "); 
	}
		
	// loop thru txt file line by line
	// create idxe_t -- word = line[0]
	// create idxe_t word_queue_p -- every 2 args is doc and doc_freq (assign accordingly);
}
