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
#include "indexio.h"


// copied from indexer.c
void freeindexentry(void *index_e) {
	indexentry = (idxe_t*) index_e; 
	
	qapply(indexentry->word_queue_p, freeDocument);
	qclose(indexentry->word_queue_p);

	free(indexentry->word);
	free(index_e);
}


void freeDocument(void *doc) {
	document = (wqe_t*) doc; 
	free(document); 
}


int main(void) {
	printf("start of test2\n");

	char filepath[] = "./test2.txt";
	hashtable_t index = hopen(20); 

	// how to create the hash table without copying our main function from indexer.c? 
	
	indexsave(index, filepath); // should've saved to new file 

	happly(index, freeindexentry);
	hclose(index); 
	
	return 0;
}
