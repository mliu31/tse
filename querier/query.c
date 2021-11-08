/* query.c --- 
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
#include <stdbool.h>
#include "queue.h"
#include "hash.h"
#include "indexio.h"


typedef struct querydocuments_queue_elem_type {
	int id; 
	int rank;
	char url[100];
	bool all_query_words_in_doc;
} qd_qe_t; 


static qd_qe_t* make_querydoc_qelem(int id) {
	qd_qe_t* querydoc_qe = (qd_qe_t*)calloc(1, sizeof(qd_qe_t));
	querydoc_qe->id = id;
	querydoc_qe->rank = 0;
	// process url
	querydoc_qe->all_query_words_in_doc = true; 

	return querydoc_qe; 
}


static void print_querydocuments_queue_elem(qd_qe_t *qd_queue_elem) {
	printf("rank:%d:doc:%d:%s", qd_queue_elem->rank, qd_queue_elem->id, qd_queue_elem->url); 
}


static void free_querydoc_qelem_type(qd_qe_t *qd_queue_elem) {
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
	int id, max_id = 7;  // depth 1
	queue_t *query_documents_queue; 
	qd_qe_t *querydoc_qelem;
	bool query_dne_in_index;
	
	index = indexload("output_depth1-7.txt");
	query_documents_queue = qopen();
	
	// take in user input
	while(true) {
	
		printf(">");
		if (fgets(input, 100, stdin) == NULL)
			break;

		token_array_size = 0;
	
		token = strtok(input, " \t\n");

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
			query_dne_in_index = false;
			
			// loop thru all documents by id and add them to our queue 
			for(id=1; id<max_id; id++) {
				if (query_dne_in_index)
					break; 

				printf("id: %d\n", id); 

				querydoc_qelem = make_querydoc_qelem(id); // QUESTION: SHOUDL IT BE THE ADDRESS?  
				
				// loop thru all query tokens 
				// if doc w/o query token, change bool all_query_words_in_doc to be false

				for(int i = 0; i < token_array_size+1; i++) {
					if(!(querydoc_qelem->all_query_words_in_doc)) //processed a word in query that the document doesn't contain
						break;
					
					indexentry = hsearch(index, search, token_array[i], strlen(token_array[i]));
					
					if(indexentry == NULL) {
						printf("%s:[query not found in index] ", token_array[i]);
						query_dne_in_index = true; 
						break; 					
					} else {
						//printf("Index element: %s\n", indexentry->word);
										
						document = qsearch(indexentry->word_queue_p, search_queue_fn, &id); 
						
						if(document == NULL) { // if document doesn't contain query word 
							// printf("%s:[word not in document] ", token_array[i]);
							querydoc_qelem->all_query_words_in_doc = false;
						} else {
							if(!(strcmp(token_array[i], "and")))
								continue; 
							else
								if(querydoc_qelem->rank == 0 || querydoc_qelem->rank > document->doc_word_freq)
									querydoc_qelem->rank = document->doc_word_freq;
							// printf("%s:%d ", token_array[i], document->doc_word_freq); 
						}
					}
				}
			
				// print minimum
				/*if(querydoc_qelem->rank != 0)
					printf("- %d\n", minimum);
				else 
					printf("\n");
				*/
				
				qput(query_documents_queue, querydoc_qelem); 
			}
			// print docs (check if all_query_words_in_doc)
		}
	}
	
	
	// TODO: free every elem in queue w qapply (create freeing fn) 
	qclose(query_documents_queue); 
	happly(index, freeindexentry);
	hclose(index);
	return 0;

}
