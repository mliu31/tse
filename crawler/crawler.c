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

int32_t pagesave(webpage_t *pagep, int id, char *dirname) {

	FILE *fp;
	char path[64];
	char name[64];

	char depth[64];
	char htmllen[64];
	char *url = webpage_getURL(pagep);
	char *html = webpage_getHTML(pagep);

	strcpy(path, dirname);	
	sprintf(name, "%d", id);
	
	sprintf(depth, "%d", webpage_getDepth(pagep));
	sprintf(htmllen, "%d", webpage_getHTMLlen(pagep));
	
	fp = fopen(strcat(path,name), "w+");

	fprintf(fp, "%s\n%s\n%s\n%s\n", url, depth, htmllen, html);
	
	fclose(fp);
	
	return 0;
}

int main(void) {
	webpage_t *rootpage, *tmp_webpage, *webpage;
	char *seedurl, *url;
	int pos = 0;
	int depth = 0;
	int maxdepth;
	char *result;
	char* directory;
	queue_t *urls_to_visit_qp;
	hashtable_t *visited_urls_htp;
	char* visited_url;
	int id = 1, counter = 0; 
	seedurl = "https://thayer.github.io/engs50/";
	directory = "../pages/";
	maxdepth = 2;

	rootpage = webpage_new(seedurl, depth, NULL);

	urls_to_visit_qp = qopen();
	visited_urls_htp = hopen(10);

	hput(visited_urls_htp, seedurl, seedurl, strlen(seedurl));
	qput(urls_to_visit_qp, rootpage);

	while(webpage = qget(urls_to_visit_qp)) {

		if(!webpage_fetch(webpage)) {
			printf("Failed to fetch webpage: %s\n", webpage_getURL(webpage));
			continue; 
		}

		if(hsearch(visited_urls_htp, search, webpage_getURL(webpage), strlen(webpage_getURL(webpage))) == NULL) {
			pagesave(webpage, id, directory);
			id = id + 1;
		}
		if(webpage_getDepth(webpage) < maxdepth) {
			depth = webpage_getDepth(webpage) + 1;

			while ((pos = webpage_getNextURL(webpage, pos, &result)) > 0) {
				if(IsInternalURL(result)) {
					//visited_url = NULL;
					//visited_url = (char*)malloc(200*sizeof(char));
					tmp_webpage = webpage_new(result, depth, NULL);
					// visited_url = webpage_getURL(tmp_webpage);

					// printf("    result: %s; %s\n", result, webpage_getURL(tmp_webpage)); //, visited_url);

					strcpy(visited_url, webpage_getURL(tmp_webpage));
					
					if(hsearch(visited_urls_htp, search, visited_url, strlen(visited_url)) == NULL) {
						hput(visited_urls_htp, visited_url, visited_url, strlen(visited_url));
						qput(urls_to_visit_qp, tmp_webpage);
						counter = counter + 1;
						printf("counter: %d\n", counter);
						
					} else free(tmp_webpage);
					

				}
				free(result);
			}
			// free(visited_url);			
		}

		
		printf("depth after increment: %d\n", depth);
		
		pos = 0;
		printf("***********************\n"); 
		qapply(urls_to_visit_qp, printurl);
		printf("***********************\n"); 
		//qapply(urls_to_visit_qp, webpage_delete);
		

		webpage_delete(webpage);
		
	}
	/*	
	while ((pos = webpage_getNextURL(webpage, pos, &result)) > 0) {
		if(IsInternalURL(result)) {
			tmp_webpage = webpage_new(result, 0, NULL);
			visited_url = webpage_getURL(tmp_webpage);
			
			if(hsearch(visited_urls_htp, search, visited_url, strlen(visited_url)) == NULL) {
				hput(visited_urls_htp, visited_url, visited_url, strlen(visited_url));
				qput(urls_to_visit_qp, tmp_webpage);
			} else free(tmp_webpage);

		}
		free(result);
		}

	free(visited_url);
	*/
	if(qget(urls_to_visit_qp) != NULL) {

		printf("queue not empty\n");

	}
	qclose(urls_to_visit_qp);
	hclose(visited_urls_htp);
 
	exit(EXIT_SUCCESS);
}
