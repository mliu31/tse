/* pageio.c --- 
 * 
 * 
 * Author: Agon Hoxha
 * Created: Mon Oct 25 17:08:41 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pageio.h"
#include "webpage.h"

int32_t pagesave(webpage_t *pagep, int id, char * dirnm) {
	FILE *fp;
	char path[64];
	char name[64];

	char depth[64];
	char htmllen[64];
	char *url = webpage_getURL(pagep);
	char *html = webpage_getHTML(pagep);

	strcpy(path, dirnm);
	sprintf(name, "%d", id);

	sprintf(depth, "%d", webpage_getDepth(pagep));
	sprintf(htmllen, "%d", webpage_getHTMLlen(pagep));

	fp = fopen(strcat(path,name), "w+");

	fprintf(fp, "%s\n%s\n%s\n%s\n", url, depth, htmllen, html);

	fclose(fp);

	return 0;
}

/* loads numbered filename <id> in directory <dirnm> into a new webpage and returns it if valid; returns null otherwise 
*/
webpage_t* pageload(int id, char *dirnm) {
	FILE *fp; 
	char filename[100];
	char filepath[100];
	char url[100];
	char *html;
	int depthint; 
	int charint; 
	char* endp;
	
	sprintf(filename, "%d", id);
	strcpy(filepath, dirnm);
	
	fp = fopen(strcat(filepath, filename), "r");
	
	fscanf(fp, "%s\n", url);
	fscanf(fp, "%d\n", &depthint);
	fscanf(fp, "%d\n", &charint);


	html = calloc(charint + 1, sizeof(char)); 

	// while(fscan(fp, "%s", html) != EOF) {};
	int i=0;
	for(i=0; i < charint; i++) {
		html[i] = fgetc(fp);
	}
	html[i] = '\0';
	
	webpage_t *loadwebpg = webpage_new(url, depthint, html); 
	
	fclose(fp); 

	return loadwebpg;
}

/*int main(void) {
	pageload(82, "../pages/");
	return 0; 
}*/
