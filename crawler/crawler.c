/* crawler.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Sun Oct 17 17:52:25 2021 (-0400)
 * Version: 1
 * 
 * Description: 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <queue.h>
#include <hash.h>
#include <webpage.h>


int main(void) {
	webpage_t *webpage;
	char *url;

	url = "https://thayer.github.io/engs50/";
		
	webpage = webpage_new(url, 0, NULL); 

	if(webpage_fetch(webpage)) {
		char *html = webpage_getHTML(webpage);
		printf("Found html: %s\n", html);
		exit(EXIT_SUCCESS); 
	}
	exit(EXIT_FAILURE); 
	
	webpage_delete(webpage); 
	exit(EXIT_SUCCESS);
}
