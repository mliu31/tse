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
	// loop thru txt file line by line
	// create idxe_t -- word = line[0]
	// create idxe_t word_queue_p -- every 2 args is doc and doc_freq (assign accordingly);
}
