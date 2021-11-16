/* indexio.h --- 
 * 
 * 
 * Author: Megan Y. Liu
 * Created: Fri Oct 29 10:13:31 2021 (-0400)
 * Version: 
 * 
 * Description: 
 * header file for indexio with two functions (1) indexsave() and (2) indexload() 
 * 
 * (1) save an index to a named file indexnm following the synatax <word> <docID1> count<1> <docID2> <count2> ... <docIDN> <countN>
 * (2) load a file from indexnm in the above syntax and returns a hashtable index  
 */

#include <pthread.h>
#include "lhash.h"

typedef struct index_entry_t {
	char *word;
	queue_t *word_queue_p;
} idxe_t;


typedef struct word_queue_entry_t {
	int doc_id;
	int doc_word_freq;
} wqe_t;


/*typedef struct locked_hashtable {
	hashtable_t *hashtable;
	pthread_mutex_t *mutex;	
} lht_t; 
*/

/* 
	 in: hashtable index
	 out: output to indexnm file in the format described above 
 */
void indexsave(hashtable_t *index, char *filepath); 

/* 
	 in: locked hashtable index
	 out: output to indexnm file in the format described above 
 */
void lindexsave(lht_t *index, char *filepath); 


/*
	in: indexnm file in the format described above 
	out: hashtable index of the data in indexm
*/
hashtable_t* indexload(char *filepath); 
