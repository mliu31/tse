/* lhash_test2.c --- 
 * 
 * 
 * Author: Alejandro A. Lopez Cochachi
 * Created: Sat Nov 13 21:36:37 2021 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdlib.h>                                                             
#include <stdio.h>                                                              
#include <string.h>
#include <unistd.h>  
#include <pthread.h>                                                            
#include "lhash.h"

typedef struct str_type {                                                       
  char* str;                                                                    
} str_t;                                                                        
                                                                                
str_t* makestr(char* input_str) {                                               
  str_t* something = (str_t*) calloc(1, sizeof(str_t));                         
  something->str = input_str;                                                   
                                                                                
  return something;                                                             
}                                                                               
                                                                                
void printe(void* elem) {                                                       
  puts((char*)elem);                                                            
}                                                                               
                                                                                
bool keysearch(void* elem, const void* key) {                                   
                                                                                
  char *element = (char*)elem;                                                  
  char *actual_key = (char*)key;                                                
                                                                                
  if(strcmp(element, actual_key) == 0)                                          
    return true;                                                                
  return false;                                                                 
                                                                                
}

void* threadProcessPut(void* locked_hash) {                                    
                                                                                
  lht_t *lhp = (lht_t*) locked_hash;                                      
  /*                                                                            
  if (lqput(lq, 3) != 0)                                                        
    return -1;                                                                  
  return 0;                                                                     
  */                                                                            
  pthread_mutex_lock(lhp->mutex);                                                
  printf("entered threada1 func\n");                                            
  sleep(5);                                                                     
  printf("exited threada1 func\n");                                             
  pthread_mutex_unlock(lhp->mutex);                                              
                                                                                
  return 0;                                                                     
}

void* threadProcessGet(void* locked_hash) {                                    
  lht_t *lhp = (lht_t*) locked_hash;                                      
  // lqget(lq);                                                                 
                                                                                
  pthread_mutex_lock(lhp->mutex);                                                
  printf("entered thread2 func\n");                                             
  sleep(2);                                                                     
  printf("exited thread2 func\n");                                              
  pthread_mutex_unlock(lhp->mutex);                                              
                                                                                
  return 0;                                                                     
} 

int main() {
	pthread_t tid1, tid2;  
  lht_t* lhp = lhopen(10);                                                              
  //char* message;                                                                
  //str_t* htentry;

	if(pthread_create(&tid2, NULL, threadProcessGet, lhp) != 0) {                 
    printf("thread 2 failed to create");                                        
    exit(EXIT_FAILURE);                                                         
  }                                                                             
  if(pthread_create(&tid1, NULL, threadProcessPut, lhp) != 0) {                 
    printf("thread 1 failed to create");                                        
    exit(EXIT_FAILURE);                                                         
  }                                                                             
                                                                                
  printf("Creations finished\n");                                               
                                                                                
  if(pthread_join(tid1,NULL) != 0)                                              
    exit(EXIT_FAILURE);                                                         
  if(pthread_join(tid2,NULL) != 0)                                              
    exit(EXIT_FAILURE);                                                         
                        
                                                                                
  /*lhp = lhopen(10);                                                             
  message = "hello world";                                                      
  htentry = makestr(message);                                                   
                                                                                
  lhput(lhp, htentry->str, "hello world", 11);                                  
                                                                                
  printf("locked hash: \n");                                                    
  lhapply(lhp, printe);                                                         
                                                                                
  char* search_result = (char*)lhsearch(lhp, keysearch, "hello world", 11);     
                                                                                
  if(strcmp(search_result, "hello world") != 0) {                               
    printf("lhsearch not working\n");                                           
    exit(EXIT_FAILURE);                                                         
  }                                                                             
                                                                                
  char* remove_result = (char*)lhremove(lhp, keysearch, "hello world", 11);     

	if(strcmp(remove_result, "hello world") != 0){                                
    printf("lhremove not working!\n");                                          
    exit(EXIT_FAILURE);                                                         
  }                                                                             
  */                                                                              
  //printf("Final hash: \n");                                                     
  //lhapply(lhp, printe);                                                         
  //free(htentry);                                                                
  lhclose(lhp);                                                                 
  exit(EXIT_SUCCESS);                                                           
                                                                                
} 
