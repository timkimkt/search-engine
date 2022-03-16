/*
 * index.c - CS50 'index' module
 * 
 * see index.h for more information
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab5
 * May, 2021 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "counters.h"
#include "set.h"
#include "mem.h"
#include "index.h"

/**************** file-local global variables ****************/
/* none */

/***************** global types *******************/
typedef struct index {
  hashtable_t* htable;           // pointer to hashtable
} index_t;

/**************** local functions ****************/
/* not visible outside this file */
static void itemdelete (void* item);
static void wordPrint(void* arg, const char* key, void* item);
static void countPrint(void* arg, const int key, const int count);

/***************** index_new() *******************/
/* see index.h for description */
index_t*
index_new(const int slots)
{
  if (slots <= 0) {
    return NULL;
  }
  // intialize contents of index
  index_t* idx = mem_malloc_assert((sizeof(idx)), "error creating index");
  idx->htable = hashtable_new(slots);
  return idx;
}

/***************** index_delete()  *******************/
/* see index.h for description */
void
index_delete(index_t* idx)
{
  if (idx == NULL) {
    return;
  } else {
    hashtable_delete(idx->htable, itemdelete);
  }
  mem_free(idx);
}

/***************** itemdelete()  *******************/
/* calls counters_delete on each hashtable itemn */
static void
itemdelete (void* item) {
  if (item != NULL) {
    counters_delete(item);
  }
}

/***************** index_find()  *******************/
/* see index.h for description */
counters_t*
index_find(index_t* idx, const char* key)
{
  if (idx == NULL || key == NULL ) {
    return NULL;
  } 
  return hashtable_find(idx->htable, key);
}

/***************** index_insert()  *******************/
/* see index.h for description */
bool
index_insert(index_t* idx, char* word, counters_t* cntr)
{
  if (idx == NULL || word == NULL || cntr == NULL ){
    return NULL;
  }  
  return hashtable_insert(idx->htable, word, cntr);
}

/***************** index_save()  *******************/
/* see index.h for description */
void
index_save(index_t* idx, char* indexFilename)
{
  FILE* fp = NULL;
  if ((fp = fopen(indexFilename, "w")) == NULL || idx == NULL) {
    return;
  }
  hashtable_iterate(idx->htable, fp, wordPrint);
  fclose(fp);
}

/***************** wordPrint()  *******************/
/* prints word and iterates counters for each word */
static void
wordPrint(void* arg, const char* key, void* item)
{
  fprintf(arg, "%s", key);
  counters_iterate(item, arg, countPrint);
  fprintf(arg, "\n");
}

/***************** countPrint()  *******************/
/* prints docID, count pair for each word in index */
static void
countPrint(void* arg, const int key, const int count) {
  fprintf(arg, " %d %d", key, count);
  return;
}

/***************** index_load()  *******************/
/* see index.h for description */
index_t*
indexLoad(char* indexFilename)
{
  FILE* fp = NULL;
  if ((fp = fopen(indexFilename, "r")) == NULL) {
    return NULL;
  }
  
  // count number of lines 
  int numLines = 0;  
  numLines = file_numLines(fp);
  
  const int index_size = numLines + 1;
  index_t* index = index_new(index_size);
 
  // scan the file and add to index 
  char* word = NULL;
  for (int i = 0; i < numLines; i++) {
    word = file_readWord(fp);
    int docID, count;
    counters_t* counter = mem_assert(counters_new(), "cannot create counters");
    // keep scanning until there is a valid pair 
    while (fscanf(fp, "%d %d ", &docID, &count) == 2) {
      // set count and add to index
      counters_set(counter, docID, count);
      index_insert(index, word, counter);
    }
    free(word);
  }
  // close index file
  fclose(fp);

  return index;
  
}
