/* crawler.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Sun Oct 17 17:52:25 2021 (-0400)
 * Version: 1
 * 
 * Description: 
 * crawler implemented in C that crawls from a seed url to a specified depth 
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

	if(strcmp(ep, sp) == 0) {
		return true;
	}
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

void freefunction(void *freed) {

	char *thing = (char*)freed;
	if(thing != NULL) {
		free(thing);
	}

}

int main(void) {
	webpage_t *rootpage, *tmp_webpage, *webpage;
	char *seedurl, *visited_url; 
	int maxdepth, pos=0, depth=0, id=1; 
	char *result, *directory;
	queue_t *urls_to_visit_qp;
	hashtable_t *visited_urls_htp;

	seedurl = "https://thayer.github.io/engs50/";
	directory = "../pages/";
	maxdepth = 2;

	rootpage = webpage_new(seedurl, depth, NULL);

	urls_to_visit_qp = qopen();
	visited_urls_htp = hopen(10);

	visited_url = NULL;
	visited_url = (char*)calloc(200, sizeof(char));
	strcpy(visited_url, seedurl);
	
	qput(urls_to_visit_qp, rootpage);
	hput(visited_urls_htp, visited_url, visited_url, strlen(visited_url));

	while((webpage = qget(urls_to_visit_qp))) {
		if(!webpage_fetch(webpage)) {
			printf("Failed to fetch webpage: %s\n", webpage_getURL(webpage));
			continue; 
		}
		
		pagesave(webpage, id, directory);

		printf("id: %d\n", id);
		id = id + 1;
				
		if(webpage_getDepth(webpage) < maxdepth) {
			depth = webpage_getDepth(webpage) + 1;

			while ((pos = webpage_getNextURL(webpage, pos, &result)) > 0) {
				if(IsInternalURL(result)) {
					tmp_webpage = webpage_new(result, depth, NULL);					

					visited_url = NULL;
					visited_url = (char*)calloc(200, sizeof(char));
					// visited_url = (char*)malloc(sizeof(webpage_getURL(tmp_webpage))+1);
					strcpy(visited_url, webpage_getURL(tmp_webpage));
					
					/*
					if(hsearch(visited_urls_htp, search, webpage_getURL(tmp_webpage), strlen(webpage_getURL(tmp_webpage))) == NULL) {
						hput(visited_urls_htp, webpage_getURL(tmp_webpage), webpage_getURL(tmp_webpage), strlen(webpage_getURL(tmp_webpage)));
						qput(urls_to_visit_qp, tmp_webpage); 
					} else 
						webpage_delete(tmp_webpage);
					*/
					
					if(hsearch(visited_urls_htp, search, visited_url, strlen(visited_url)) == NULL) {
						hput(visited_urls_htp, visited_url, visited_url, strlen(visited_url));
						qput(urls_to_visit_qp, tmp_webpage); 
						printf(" %s  ==================== put into queue and hash table\n", visited_url);
						// printf(" %s  ==================== put into queue and hash table\n", webpage_getURL(tmp_webpage));
					} else {
						webpage_delete(tmp_webpage);
						free(visited_url);
					}
					
				}
				free(result);
			}
			// free(visited_url);	
		}
		
		printf("depth of popped off webpg from queue: %d\n", depth);
		
		pos = 0;
		printf("**********************************\n"); 
		qapply(urls_to_visit_qp, printurl);
		printf("**********************************\n"); 
		//qapply(urls_to_visit_qp, webpage_delete);
		
		webpage_delete(webpage);
	}

	happly(visited_urls_htp, freefunction);
	qclose(urls_to_visit_qp);
	hclose(visited_urls_htp);

	exit(EXIT_SUCCESS); 
}
