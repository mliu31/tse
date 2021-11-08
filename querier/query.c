od/* query.c --- 
 * 
 * 
 * Author: Agon Hoxha
 * Created: Tue Nov  2 11:01:13 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "queue.h"
#include "hash.h"
#include "indexio.h"


typedef struct querydocuments_queue_elem_type {
	wqe_t *document;
	int rank;
	char url[100];
	bool all_query_words_in_doc = true; 
} qd_qe_t; 


static void print_querydocuments_queue_elem(qd_qe_t *qd_queue_elem) {
	printf("rank:%d:doc:%d:%s", qd_queue_elem->rank, qd_queue_elem->document->doc_id, qd_queue_elem->url); 
}

static void free_querydoc_qelem_type(qd_qe_t *qd_queue_elem) {
	free(qd_queue_elem->document);
	free(qd_queue_elem->url);
	free(qd_queue_elem); 
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


static bool search(void* elementp, const void* searchkeyp) {
	idxe_t *ep = (idxe_t*)elementp;
	char *sp = (char*)searchkeyp;

	if(strcmp(ep->word, sp) == 0) {
		return true;
	}
	return false;
}


static bool search_queue_fn(void* elementp, const void* searchkeyp) {
	wqe_t *ep = (wqe_t*)elementp;
	int *sp = (int*)searchkeyp;

	if(ep->doc_id == *sp) {
		return true;
	}
	return false;
}


int main(void) {
	char input[100];
	char *token;
	char *token_array[100];
	int token_array_size;
	bool is_invalid_query = false; 
	hashtable_t *index;
	idxe_t *indexentry;
	wqe_t *document;
	int id = 1, max_id = 7;  // depth 1
	int minimum;
	queue_t *query_documents_queue; 
	bool all_query_words_in_doc;
	
	index = indexload("output_depth1-7.txt");
	query_documents_queue = qopen();
	
	// take in user input
	while(true) {
	
		printf(">");
		if (fgets(input, 100, stdin) == NULL)
			break;

		token_array_size = 0;
	
		token = strtok(input, " \t\n");
		minimum = 0;

		// validate user input 
		while(token != NULL) {
			for(int i=0; i<strlen(token); i++) {
				if((!(isalpha(token[i]))) && (token[i] != '\n')) {					
					is_invalid_query = true; 
					printf("[invalid query]\n");
					break;
				}	else {
					token[i] = tolower(token[i]);
				}
			}

			if(is_invalid_query)
				break;

			token_array[token_array_size] = token;
			//printf("word: %s\n", token_array[token_array_size]);
		
			token_array_size++;
			//printf("count: %d\n", token_array_size);

			token = strtok(NULL, " \t\n");
		}

		// process user input wrt index hashtable -- create queue of docs that contain all the words in the query
		if(!is_invalid_query) {
			// PSEUDOCODE--not a clean soln tho
			// get (valid) indexentry's word_queue (elem containing doc_id and doc_freq) from the first token and put all elem in queue w elem type of querydocuments_queue_elem_type (need to malloc)
			// qapply the recently-made queue and check that the rest of the tokens are in the docs (for loop starting at i=1 to i<token_array_size)
		 	    // if they aren't in the doc, modify boolean all_query_words_in_doc = false
			// print documents in the structure specified in the instructions for elems with all_query_words_in_doc = true
			// free elems and queue 

			for(int i = 0; i < token_array_size; i++) {
				indexentry = hsearch(index, search, token_array[i], strlen(token_array[i]));

				if(indexentry == NULL) {
				  printf("%s:[query not found in index] ", token_array[i]);
					all_query_words_in_doc = false;
					break; 					
				} else {
					//printf("Index element: %s\n", indexentry->word);
					
					// loop thru all documents (by id) 
					//					for(int i=0; i<max_id; i++) { 
						document = qsearch(indexentry->word_queue_p, search_queue_fn, &id); 

						if(document == NULL) { // if document doesn't contain query word 
							printf("%s:[word not in document] ", token_array[i]);
						} else {
							if(!(strcmp(token_array[i], "and")))
								continue; 
							else
								if(minimum == 0 || minimum > document->doc_word_freq)
									minimum = document->doc_word_freq;
							//printf("Word count is: %d\n", document->doc_word_freq);
							printf("%s:%d ", token_array[i], document->doc_word_freq); 
						}
						//						id++;
						//}
				}
			}

			// print minimum
			if(minimum != 0)
				printf("- %d\n", minimum);
			else {
				minimum = 0;
				printf("\n");


				// print doc queue 
			}
		}
	}

	qclose(query_documents_queue); 
	happly(index, freeindexentry);
	hclose(index);
	return 0;

}
