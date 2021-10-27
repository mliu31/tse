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
#include <ctype.h>
#include <pageio.h>
#include <webpage.h>
#include <hash.h>
#include <string.h>


int sum = 0;


typedef struct index_entry_t {
	char *word;
	queue_t *word_queue_p; 
} id_t;


typedef struct word_queue_entry_t {
	int doc_id;
	int doc_word_freq; 
} wqe_t; 


id_t* makeindexentry(char *word) {
	id_t* indexentry = malloc(sizeof(id_t));

	indexentry->word = word;
	indexentry->freq = 1;

	return indexentry; 
}


void incrementindexentry(id_t *indexentry) {
	indexentry->freq = (indexentry->freq) + 1;
}


void printindexentry(void *indexentry) {
	id_t *index_e = (id_t*)(indexentry);
	printf("%s: %d\n", index_e->word, index_e->freq); 
}


void freeindexentry(void *indexentry) {
	id_t *index_e = (id_t*)(indexentry);
	free(index_e->word);
	free(index_e);
}


void sumofindexentries(void *indexentry) {
	id_t *index_e = (id_t*)(indexentry);
	int *sum_p = &sum;
	
	*sum_p = *sum_p + index_e->freq;
}


int NormalizeWord(char **wordptr) {
	char *word = *wordptr;
	int i = 0;

	while(word[i] != '\0') {
		if(isalpha(word[i]) == 0) {
			free(word);
			return -1;
		}
		word[i] = ((char)(((int)tolower(word[i]))));
		i++;
	}

	if(i < 4) {
		free(word);
		return -1;
	}
	return 0;
}


bool search(void* elementp, const void* searchkeyp) {
	id_t *ep = (id_t*)elementp;
	char *sp = (char*)searchkeyp;

	if(strcmp(ep->word, sp) == 0) {
		return true;
	}
	return false;
}


int main(void) {
	webpage_t *loadedpage;
	hashtable_t *hashtable;
	id_t *indexentry;
	char *word;
	int pos = 0;

	loadedpage = pageload(1, "../pages/");
	hashtable = hopen(20);
	
	while(true) {
		pos = webpage_getNextWord(loadedpage, pos, &word);

		if(pos == -1) {
			break;
		}
		
		if(NormalizeWord(&word) == 0) {
			indexentry = hsearch(hashtable, search, word, strlen(word));
			if(indexentry == NULL) {
				indexentry = makeindexentry(word);
				hput(hashtable, indexentry, word, strlen(word));
			} else {
				incrementindexentry(indexentry);
				free(word);
			}
		}
	}
	
	happly(hashtable, printindexentry);
	happly(hashtable, sumofindexentries);
	printf("sum: %d\n", sum);
	
	happly(hashtable, freeindexentry);
	hclose(hashtable);
	webpage_delete(loadedpage);

	return 0;
}
