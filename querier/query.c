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


static bool search(void* elementp, const void* searchkeyp) {
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
	wqe_t *result;

	while(document = (qget(small_box) != NULL)) {

		result = qsearch(big_box, document->doc_id);

		if(result == NULL)
			qput(big_box, document);
		else
			result->doc_word_freq += document->doc_word_freq;

		qput(backup, document);

	}

	qconcat(small_box, backup);

}
	

void qintersection(queue_t *big_box, queue_t *small_box) {

	queue_t *backup = qopen();
	wqe_t *document;
	wqe_t *result;

	while(document = (qget(small_box) != NULL)) {

		result = qsearch(big_box, document->doc_id);

		if(result == NULL)
			qput(big_box, document);
		else {
			if(result->doc_word_freq > document->doc_word_freq)
				result->doc_word_freq = document->doc_word_freq;
		}
		
		qput(backup, document);

	}

	qconcat(small_box, backup);

}

int main(void) {
	char input[100];
	char *token;
	char *token_array[100];
	int token_array_size;
	bool is_invalid_query = false; 
	hashtable_t *index;
	queue_t *big_box, *small_box;
	idxe_t *indexentry;
	wqe_t *document;
	int id = 1;
	int minimum;
	
	index = indexload("output_depth1-7.txt");
	
	while(true) {
	
		printf(">");
		if (fgets(input, 100, stdin) == NULL)
			break;

		token_array_size = 0;
	
		token = strtok(input, " \t\n");
		minimum = 0;
		
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
		
		if(!is_invalid_query) {

			big_box = qopen();
			small_box = qopen();
			
			for(int i = 0; i < token_array_size; i++) {
				indexentry = hsearch(index, search, token_array[i], strlen(token_array[i]));

				if(strcmp(indexentry->word, "or")) {
						
					qunion(big_box, small_box);
					
						
				}

				
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
				/*if (token_array_size == 1)
					printf("%s\n", token_array[i]);
				else if (i == token_array_size-1) 
					printf("%s", token_array[i]);
				else
					printf("%s ", token_array[i]);
					}*/
			}

			if(minimum != 0)
				printf("- %d\n", minimum);
			else {
				minimum = 0;
				printf("\n");
			}
		}
	}
	happly(index, freeindexentry);
	hclose(index);
	return 0;

}
