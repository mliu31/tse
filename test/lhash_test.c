/* lhash_test.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Sat Nov 13 20:49:20 2021 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "lhash.h"

typedef struct str_type {
	char* str;
} str_t;

str_t* makestr(char* input_str) {
	str_t* something = (str_t*) calloc(1, sizeof(str_t));
	something->str = input_str;

	return something;
}

void printe(void* elem) {
	puts((char*)elem);
}

bool keysearch(void* elem, const void* key) {

	char *element = (char*)elem;
	char *actual_key = (char*)key;

	if(strcmp(element, actual_key) == 0)   
		return true;
	return false;

}

int main() {
	lht_t* lhp;
	char* message;
	str_t* htentry;
	
	lhp = lhopen(10);
	message = "hello world";
	htentry = makestr(message);

	lhput(lhp, htentry->str, "hello world", 11);

	printf("locked hash: \n");
	lhapply(lhp, printe);

	char* search_result = (char*)lhsearch(lhp, keysearch, "hello world", 11);

	if(strcmp(search_result, "hello world") != 0) {
		printf("lhsearch not working\n");
		exit(EXIT_FAILURE);
	}
	
	char* remove_result = (char*)lhremove(lhp, keysearch, "hello world", 11);

	if(strcmp(remove_result, "hello world") != 0){
		printf("lhremove not working!\n");
		exit(EXIT_FAILURE);
	}

	printf("Final hash: \n");
	lhapply(lhp, printe);
	free(htentry);
	lhclose(lhp);
	exit(EXIT_SUCCESS);
		
}
