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
#include "lhash.h"


FILE *file;

static wqe_t* makeQueueEntry(int id, int freq) {
	wqe_t* queue_entry = (wqe_t*)calloc(1, sizeof(wqe_t));

	queue_entry->doc_id = id;
	queue_entry->doc_word_freq = freq;

	return queue_entry;
}


static idxe_t* makeindexentry(char *word) {
	idxe_t* indexentry = malloc(sizeof(idxe_t));
	
	indexentry->word = word;

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

void lindexsave(lht_t *lindex, char *filepath) { 
	file = fopen(filepath, "w+");
	
	lhapply(lindex, printindexentry); 
	
	fclose(file);

} 


hashtable_t* indexload(char *filepath) {
	
	file = fopen(filepath, "r");
	
	char *word;
	int doc, doc_freq;
	hashtable_t* index = hopen(20);

	char line[640];
	idxe_t *indexentry;
	wqe_t *queueentry;
	int offset, readcharcount;
	
	// use fgets to loop thru each line
	// for each returned str from fgets, use strtok and use " " as delimiter  -- condition of strtok != NULL

	
	while(fgets(line, 1000, file) != NULL) {

		offset = 0;
		readcharcount = 0;
		
		word = (char*)calloc(200, sizeof(char));
		sscanf(line, "%s%*c%n", word, &readcharcount);
		
		offset += readcharcount;
	
		indexentry = makeindexentry(word);
		indexentry->word_queue_p = qopen();

		while(sscanf(line + offset, "%d%*c%d%*c%n", &doc, &doc_freq, &readcharcount) != EOF) {
			
			//sscanf(line + offset, "%d%n", &doc, &readcharcount);
			//printf("entered while loop\n");
			//printf("doc_id: %d", doc);
			offset += readcharcount;
			queueentry = makeQueueEntry(doc, doc_freq);
			qput(indexentry->word_queue_p, queueentry);
		
		}
		//	}
		//}
		hput(index, indexentry, indexentry->word, strlen(indexentry->word));

	}
	fclose(file);

	return index;
	// loop thru txt file line by line
	// create idxe_t -- word = line[0]
	// create idxe_t word_queue_p -- every 2 args is doc and doc_freq (assign accordingly);
}
