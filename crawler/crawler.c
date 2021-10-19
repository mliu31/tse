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
	printf("%s-->\n", webpage_getURL((webpage_t*)pagep));
}

bool search(void* elementp, const void* searchkeyp) {

	char *ep = (char*)elementp;
	char *sp = (char*)searchkeyp;

	if(strcmp(ep, sp) == 0)
		return true;

	return false;
	
}
/*
int32_t pagesave(webpage_t *pagep, int id, char *dirname) {

	FILE *fp;
	char *path = "../pages/";
	char *name;
	
	sprintf(name, "%d", id);
	
	fp = fopen(strcat(path,name), "w+"); 

	return 0;
	}*/

int main(void) {
	webpage_t *webpage, *tmp_webpage;
	char *url;
	int pos = 0;
	char *result;
	queue_t *urls_to_visit_qp;
	hashtable_t *visited_urls_htp;
	char* visited_url;
	
	url = "https://thayer.github.io/engs50/";
		
	webpage = webpage_new(url, 0, NULL);

	//pagesave(webpage, 1, "../pages/");

	if(webpage_fetch(webpage)) {
		char *html = webpage_getHTML(webpage);
		//		printf("Found html: %s\n", html);
	}

	urls_to_visit_qp = qopen();
	visited_urls_htp = hopen(10);
	
	while ((pos = webpage_getNextURL(webpage, pos, &result)) > 0) {
		if(IsInternalURL(result)) {
			//printf("[INTERNAL]: %s\n", result);
			tmp_webpage = webpage_new(result, 0, NULL);
			visited_url = webpage_getURL(tmp_webpage);
			
			if(hsearch(visited_urls_htp, search, visited_url, strlen(visited_url)) == NULL) {
				hput(visited_urls_htp, visited_url, visited_url, strlen(visited_url));
				qput(urls_to_visit_qp, tmp_webpage);
			} else free(tmp_webpage);

		}/* else {
			printf("[EXTERNAL]: %s\n", result);		
			}*/
		free(result);
	}

	free(visited_url);
	qapply(urls_to_visit_qp, printurl);
	qapply(urls_to_visit_qp, webpage_delete);
	qclose(urls_to_visit_qp);
	hclose(visited_urls_htp);
	
	webpage_delete(webpage);
	exit(EXIT_SUCCESS);
}
