
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

int main(void) {
 
	char input[100];
	char *token;
	//char **token_array;
	//char **ptr;
	int token_array_size = 0;
	
	printf(">");
	scanf("%s", input);
	printf("input: %s\n", input);

	token = strtok(input, " ");
	
	//token_array = &token;
	//ptr = token_array;

	while(token != NULL) {
		
		//ptr[token_array_size] = token;
		//printf("%s\n", ptr[token_array_size]);
		
		token_array_size++;
		printf("count: %d", token_array_size);
		//ptr++;

		token = strtok(NULL, " ");
		
	}


	/*ptr = token_array;

	for(int i = 0; i < token_array_size; i++) {

		printf("%s ", *ptr);
		ptr++;

	}

	printf("\n");*/

	return 0;

}
