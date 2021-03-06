
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


int main(void) {
 
	char input[100];
	char *token;
	char *token_array[100];
	int token_array_size;
	bool is_invalid_query = false; 
	int scanf_output;
	hashtable_t *index;

  index = indexload("./1.txt");

	while(true) {
	
		printf(">");
		if (fgets(input, 100, stdin) == NULL)
			break;

		token_array_size = 0;
	
		token = strtok(input, " \t\n");
	
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

			token = strtok(NULL, " \t");
		
		}

		if(!is_invalid_query) {
			for(int i = 0; i < token_array_size; i++) {
				if (token_array_size == 1)
					printf("%s\n", token_array[i]);
				else if (i == token_array_size-1) 
					printf("%s", token_array[i]);
				else
					printf("%s ", token_array[i]);
			}
		}

	}
	happly(index, freeindexentry);
	hclose(index);
	return 0;

}
