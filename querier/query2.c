
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
	int non_a_flag;
	int scanf_output;
	hashtable_t *index;

	index = hopen(20);
	index = indexload("./1.txt");

	while(true) {
	
		printf(">");
		fgets(input, 100, stdin);

		token_array_size = 0;
		non_a_flag = 0;
	
		token = strtok(input, " \t");
	
		//token_array = (char**)(malloc(sizeof(char**)));
	
		while(token != NULL) {

			for(int i = 0; i < strlen(token); i++) {

				if((!(isalpha(token[i]))) && (token[i] != '\n')) {					
					non_a_flag = 1;
					printf("[invalid query]\n");
					break;
					
				}	else {
					token[i] = tolower(token[i]);
				}
				
			}

			if(non_a_flag == 1)
				break;

			token_array[token_array_size] = token;
			//printf("word: %s\n", token_array[token_array_size]);
		
			token_array_size++;
			//printf("count: %d\n", token_array_size);

			token = strtok(NULL, " \t");
		
		}

		if(non_a_flag == 0) {
			for(int i = 0; i < token_array_size; i++) {

				printf("%s ", token_array[i]);

			}
		}

	}
	happly(index, freeindexentry);
	hclose(index);
	return 0;

}
