
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

int main(void) {
 
	char input[100];
	char *token;
	char *token_array[100];
	int token_array_size;
	int non_a_flag;
	int scanf_output;

	while(true) {
	
		printf(">");
		fgets(input, 100, stdin);

		token_array_size = 0;
		non_a_flag = 0;
	
		token = strtok(input, " \t ");
	
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

			token = strtok(NULL, " ");
		
		}

		if(non_a_flag == 0) {
			for(int i = 0; i < token_array_size; i++) {

				printf("%s ", token_array[i]);

			}
		}

	}
	
	return 0;

}
