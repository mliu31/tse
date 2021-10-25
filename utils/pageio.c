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
