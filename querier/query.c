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
#include "queue.h"
#include "hash.h"
#include "indexio.h"


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


static bool search_index(void* elementp, const void* searchkeyp) {
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


void qunion(queue_t *big_box, queue_t *small_box) {
	if(big_box == NULL || small_box == NULL) return;
 
	queue_t *backup = qopen();
	wqe_t *document, *new_doc;
	wqe_t *result;

	while((document = ((wqe_t*)qget(small_box)))) {
		// printf("QUNION // documentid: %d ; docfreq: %d\n", document->doc_id, document->doc_word_freq); 
		
		result = qsearch(big_box, search_queue, (void*)(&document->doc_id));

		if(result == NULL) {
			new_doc = malloc(sizeof(wqe_t));
			new_doc->doc_id = document->doc_id;
			new_doc->doc_word_freq = document->doc_word_freq; 
			qput(big_box, new_doc);
		}
		else {
			result->doc_word_freq += document->doc_word_freq;
		}
		qput(backup, document);

	}
	qconcat(small_box, backup);
}
	

void qintersect(queue_t *big_box, queue_t *small_box) {
	if(big_box == NULL || small_box == NULL) return;
	
	queue_t *backup = qopen();
	wqe_t *document;
	wqe_t *result;

	while((document = ((wqe_t*)qget(big_box)))) {
		result = qsearch(small_box, search_queue, (void*)(&document->doc_id));
		
		if(result == NULL)
			free(document); 
		else {
			// printf("qintersect for doc %d*******************", document->doc_id);
			// printf("result freq: %d || doc freq: %d\n", result->doc_word_freq, document->doc_word_freq);
			qput(backup, document);
			if(result->doc_word_freq < document->doc_word_freq) {
				document->doc_word_freq = result->doc_word_freq; 
			}
		}
	}
	qconcat(big_box, backup);

}


void print_query_output(void* q) {
	wqe_t* q_elem = (wqe_t*)q;

	printf("docid: %d, docfreq: %d\n", q_elem->doc_id, q_elem->doc_word_freq); 
}


queue_t* process_query_tokens(int token_array_size, char **token_array, hashtable_t *index) {
	queue_t *big_box, *small_box=NULL; 
	idxe_t *indexentry;
	
	big_box = qopen();
			
	for(int i = 0; i < token_array_size; i++) {
		//		printf("TOKEN--%d: %s\n", i, token_array[i]);
		if(!(strcmp(token_array[i], "or"))) {
			// printf("#1 (if or)----------------------------------------------\n"); 
			qunion(big_box, small_box);
			qapply(small_box, freeDocument); 
			if(small_box != NULL) 
				qclose(small_box);
			small_box = NULL; 			
		} else if(strcmp(token_array[i], "and")) { // actual word in query
			// printf("#2 (actual word in query)----------------------------------------------\n"); 
			indexentry = hsearch(index, search_index, token_array[i], strlen(token_array[i]));					
			if(indexentry == NULL) {
				// 				printf("#2.1 (query word not in index)----------------------------------------------\n");

				int j = i+1;
				while(j<token_array_size) {
					if(!(strcmp(token_array[j], "or")))
						break; 
					j++; 
				}
				i = j-1; 
				// if(i == token_array_size-1) i--;
						
			} else if (small_box == NULL) {
				// 				printf("#2.2 (null small box)----------------------------------------------\n");

				small_box = qopen(); 
				qunion(small_box, indexentry->word_queue_p); 
			}
			else {
				// 				printf("#2.3 (else)----------------------------------------------\n");
				//				printf("indexentry word: %s\n", indexentry->word); 
				qintersect(small_box, indexentry->word_queue_p);
			}
		}
	}
	
	if(small_box != NULL) {
		qunion(big_box, small_box);
		qapply(small_box, freeDocument); 
		qclose(small_box); 
	}
	
	// print out everything
	qapply(big_box, print_query_output);
	return big_box; 
}


int main(void) {
	char input[100];
	char *token;
	char *token_array[100];
	int token_array_size;
	bool is_invalid_query = false; 
	hashtable_t *index;
	queue_t *big_box;  //, *small_box;
	
	index = indexload("output_depth1-7.txt");
	
	while(true) {
		// take in user input
		printf(">");
		if (fgets(input, 100, stdin) == NULL)
			break;

		token_array_size = 0;
	
		token = strtok(input, " \t\n");
		//		minimum = 0;
		
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
			// printf("word: %s\n", token_array[token_array_size]);
		
			token_array_size++;
			//printf("count: %d\n", token_array_size);

			token = strtok(NULL, " \t\n");
		}

		// processing tokens to produce output 
		if(!is_invalid_query) {
			big_box = process_query_tokens(token_array_size, token_array, index); 
			
		}
		if (big_box != NULL) {
			qapply(big_box, freeDocument); 
			qclose(big_box);
		}
	}
	happly(index, freeindexentry);
	hclose(index);
	return 0;

	}
