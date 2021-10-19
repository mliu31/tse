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
#include <string.h>
#include <queue.h>
#include <hash.h>
#include <webpage.h>

void printurl(void* pagep) {
	printf("%s\n", webpage_getURL((webpage_t*)pagep));
}

int main(void) {
	webpage_t *webpage, *tmp_webpage;
	char *url;
	int pos = 0;
	char *result;
	queue_t *urls_to_visit_qp;
	
	url = "https://thayer.github.io/engs50/";
		
	webpage = webpage_new(url, 0, NULL); 

	if(webpage_fetch(webpage)) {
		char *html = webpage_getHTML(webpage);
		//		printf("Found html: %s\n", html);
	}

	urls_to_visit_qp = qopen();
		while ((pos = webpage_getNextURL(webpage, pos, &result)) > 0) {
		if(IsInternalURL(result)) {
			printf("[INTERNAL]: %s\n", result);
			tmp_webpage = webpage_new(result, 0, NULL);
			qput(urls_to_visit_qp, tmp_webpage); 
		} else {
			printf("[EXTERNAL]: %s\n", result);		
		}
		free(result);
	}

	
	
	qapply(urls_to_visit_qp, printurl);
	qclose(urls_to_visit_qp);
	
	webpage_delete(webpage); 
	exit(EXIT_SUCCESS);
}
