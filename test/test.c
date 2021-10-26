/* test.c --- 
 * 
 * 
 * Author: Agon Hoxha
 * Created: Tue Oct 26 10:41:16 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include "webpage.h"
#include "pageio.h"

int main(void) {
	
	printf("start of test\n");
	webpage_t *loadedwebpage;
	//webpage_t *loadedwebpage2;
	
	loadedwebpage = pageload(1, "../pages2/");
	pagesave(loadedwebpage, 100, "../pages2/");
	//loadedwebpage2 = pageloadcd (100, "../pages2/");
	
	webpage_delete(loadedwebpage);
	
	return 0;
}
