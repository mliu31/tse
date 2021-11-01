/* test2.c --- testing indexio.c functions
 * 
 * 
 * Author: Megan Y Liu
 * Created: Sat Oct 30 10:41:16 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include "webpage.h"
#include "pageio.h"
#include "queue.h"
#include "hash.h"
#include "indexio.h"


void freeDocument(void *doc) {
	wqe_t *document = (wqe_t*) doc; 
	free(document); 
}

// copied from indexer.c
void freeindexentry(void *index_e) {
	idxe_t *indexentry = (idxe_t*) index_e; 
	
	qapply(indexentry->word_queue_p, freeDocument);
	qclose(indexentry->word_queue_p);

	free(indexentry->word);
	free(index_e);
}

int main(void) {
	printf("start of test2\n");

	char open_filepath[] = "./test2.txt";
	char save_filepath[] = "./test2_save.txt";
	hashtable_t *index; 

	// how to create the hash table without copying our main function from indexer.c? 
	index = indexload(open_filepath);
	indexsave(index, save_filepath); // should've saved to new file 

	happly(index, freeindexentry);
	hclose(index); 
	
	return 0;
}
