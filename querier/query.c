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

	queue_t *backup = qopen();
	wqe_t *document;
	int *doc_id;
	wqe_t *result;

	while((document = ((wqe_t*)qget(small_box)))) {
		printf("documentid: %d\n", document->doc_id); 
		
		doc_id = &(document->doc_id);
		result = qsearch(big_box, search_queue, (void*)doc_id);

		if(result == NULL)
			qput(big_box, document);
		else {
			result->doc_word_freq += document->doc_word_freq;
		}
		qput(backup, document);

	}
	qconcat(small_box, backup);
}
	

void qintersect(queue_t *big_box, queue_t *small_box) {

	queue_t *backup = qopen();
	wqe_t *document;
	int *doc_id; 
	wqe_t *result;

	while((document = ((wqe_t*)qget(small_box)))) {
		doc_id = &(document->doc_id);
		result = qsearch(big_box, search_queue, (void*)doc_id);

		if(result == NULL)
			qput(big_box, document);
		else {
			result = calloc(1, sizeof(wqe_t*));
			if(result->doc_word_freq > document->doc_word_freq)
				result->doc_word_freq = document->doc_word_freq;
		}
		qput(backup, document);
	}
	qconcat(small_box, backup); 
}


void process_query_tokens(int token_array_size, char **token_array, hashtable_t *index) {
	queue_t *big_box, *small_box=NULL; 
	idxe_t *indexentry;
	
	big_box = qopen();
			
	for(int i = 0; i < token_array_size; i++) {
		printf("TOKEN--%d: %s\n", i, token_array[i]);
		
		if(!(strcmp(token_array[i], "or"))) {
			printf("#1 (if or)----------------------------------------------\n");
			
			qunion(big_box, small_box);
			qclose(small_box);
						
		} else if(strcmp(token_array[i], "and")) { // actual word in query
			printf("#2 (actual word in query)----------------------------------------------\n"); 

			indexentry = hsearch(index, search_index, token_array[i], strlen(token_array[i]));					

			if(indexentry == NULL) {
				printf("#2.1 (query word not in index)----------------------------------------------\n");

				int j = i+1;
				while(j<token_array_size) {
					if(!(strcmp(token_array[j], "or")))
						break; 
					j++; 
				}
				i = j-1; 
				// if(i == token_array_size-1) i--;
						
			} else if (small_box == NULL) {
				printf("#2.2 (null small box)----------------------------------------------\n");

				small_box = qopen(); 
				qunion(small_box, indexentry->word_queue_p); 
			}
			else {
				printf("#2.3 (else)----------------------------------------------\n");
		
				qintersect(small_box, indexentry->word_queue_p);
			}
		}
	}
	if(small_box != NULL)
		qunion(big_box, small_box);


	// print out everything 
}


int main(void) {
	char input[100];
	char *token;
	char *token_array[100];
	int token_array_size;
	bool is_invalid_query = false; 
	hashtable_t *index;
	//	queue_t *big_box, *small_box;
	//	idxe_t *indexentry;
	//	wqe_t *document;
	//	int id = 1;
	//	int minimum;
	
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
			process_query_tokens(token_array_size, token_array, index); 
			
			/*
			big_box = qopen();
			//	small_box = qopen();
			
			for(int i = 0; i < token_array_size; i++) {

				if(strcmp(token_array[i], "or")) {
						
					qunion(big_box, small_box);
					qclose(small_box);
					// small_box = qopen(); 
						
				} else if(!(strcmp(token_array[i], "and"))) { // actual word in query

					indexentry = hsearch(index, search_index, token_array[i], strlen(token_array[i]));					

					if(indexentry == NULL) {

						while(!(strcmp(token_array[i], "or"))) {
							if(i == token_array_size) break;
							i++; 
						}
						if(i == token_array_size-1) i--;
						
												 
					} else if (small_box == NULL)
						small_box = qopen(); 
					union(small_box, indexentry->word_queue_p); 

					else
						intersect(small_box, indexentry->word_queue_p); 
				}

				if(small_box != NULL)
					union(big_box, small_box); 
				*/
				
				////////////////////
				/*
					if(indexentry == NULL) {
					printf("%s:[query not found in index] ", token_array[i]);
					} else {
					//printf("Index element: %s\n", indexentry->word);
					
					document = qsearch(indexentry->word_queue_p, search_queue, &id);

					if(document == NULL) {
					printf("%s:[word not in document] ", token_array[i]);
					} else {
					if(!(strcmp(token_array[i], "and"))) {
					// printf("%s --- continuing", token_array[i]); 
					continue; 
					}
					else
					if(minimum == 0 || minimum > document->doc_word_freq)
					minimum = document->doc_word_freq;
					//printf("Word count is: %d\n", document->doc_word_freq);
					printf("%s:%d ", token_array[i], document->doc_word_freq); 
					}
					}
				
					//if (token_array_size == 1)
					//printf("%s\n", token_array[i]);
					//else if (i == token_array_size-1) 
					//	printf("%s", token_array[i]);
					//else
					//	printf("%s ", token_array[i]);
					//	}
					}

					if(minimum != 0)
					printf("- %d\n", minimum);
					else {
					minimum = 0;
					printf("\n");
					}
				*/
			}
		}
		happly(index, freeindexentry);
		hclose(index);
		return 0;

	}
