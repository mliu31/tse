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

// copied from indexer.c
static void printindexentry(void *index_e) {
	indexentry = (idxe_t*) index_e;
	
	fprintf(file, "%s ", indexentry->word); 
	qapply(indexentry->word_queue_p, printDocument); 
	fprintf(file, "\n"); 
}

static void printDocument(void *doc) {
	document = (wqe_t*) doc; 

	fprintf(file, "%d %d ", document->doc_id, document->doc_word_freq); 
}

void indexsave(hashtable_t *index, char *filepath) {
	file = fopen(filepath, "w+");

	happly(index, printindexentry); 

	fclose(file); 
}


hashtable_t* indexload(char *filepath) {
	; 
}
