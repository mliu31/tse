/* indexer.c --- 
 * 
 * 
 * Author: Agon Hoxha
 * Created: Tue Oct 26 19:04:11 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <pageio.h>
#include <webpage.h>

int main(void) {

	webpage_t *loadedpage;
	char *word;
	int pos = 0;

	printf("Hello!\n");

	loadedpage = pageload(1, "../pages/");

	while(pos != webpage_getHTMLlen(loadedpage)) {

		pos = webpage_getNextWord(loadedpage, pos, &word);
		printf("%s\n", word);
		printf("pos: %d\n", pos);

		free(word);

	}

	webpage_delete(loadedpage);

	return 0;

}
