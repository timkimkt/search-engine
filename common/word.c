/*
 * word.c - CS50 'word' module 
 * 
 * see word.h for more information 
 *
 * Tim (Kyoung Tae) Kim, CS50 Lab5
 * May, 2021 
 */

#include <ctype.h>
#include "mem.h"
#include "string.h"

/*************** normalizeWord **********************/
/* See detailed implementation in word.h */
char*
normalizeWord (char* word){

  if (word == NULL) {
    return NULL;
  }
  
  char* word_lower = mem_malloc_assert((strlen(word)+1), "failed to create word");
  int i = 0;
  
  // converts each character to lowercase
  for (i = 0; i < strlen(word); i++) {
    word_lower[i] = tolower(word[i]);
  }
  // terminate string
  word_lower[i] = '\0';
  
  return word_lower;
}
