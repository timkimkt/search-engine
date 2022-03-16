/* 
 * index.h - header file for CS50 'index' module
 * 
 * An ‘index’ is a hashtable of counters that contain (docID count) pairs. 
 * It is a wrapper for the hashtable module with local functions that 
 * help write the index file to the given file pointer. 
 *
 * Tim (Kyoung Tae) Kim, CS50 Lab5
 * May, 2021
 */

#ifndef __index_H
#define __index_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "counters.h"
#include "set.h"
#include "mem.h"
#include "file.h"

/***************** global types *******************/
typedef struct index index_t;

/**************** functions ****************/


/***************** index_new() *******************/
/* Create a new (empty) index.
 * We return:
 *   pointer to a new index, or NULL if error.
 * We guarantee:
 *   The index is initialized empty.
 * Caller is responsible for:
 *   later calling index_delete.
 */
index_t* index_new(const int slots);

/***************** index_delete()  *******************/
/* Delete the entire index.
 * Caller provides:
 *   a valid index pointer.
 * We guarantee:
 *   we call counters_delete on each counters item.
 *   we ignore NULL index
 * Notes:
 *   the order in which items are deleted is undefined.
 */
void index_delete(index_t* idx);

/***************** index_find()  *******************/
/* Check if count for word exists in index.
 * Call provides:
 *   valid pointer to index, valid string for key
 * We return:
 *   pointer to the counter corresponding to given key, or NULL if error.
 *   NULL if index is NULL, key is NULL, or key is not found
 * Note:
 *   the index is unchanged by this operation
 */
counters_t* index_find(index_t* idx, const char* key);

/***************** index_insert()  *******************/
/* Add new word and counter to the index.
 * Caller provides:
 *   a valid index pointer and a valid word and counter.
 * We guarantee:
 *   a NULL index is ignored; a NULL word or counter is ignored.
 *   word is added as key and counter as item to hashtable.
 * Caller is responsible for:
 *   not free-ing the item as long as it remains in the index.
 */
bool index_insert(index_t* idx, char* word, counters_t* cntr);

/***************** index_save()  *******************/
/* iterates through hashtable to print word and docID, count pair
 * Call provides:
 *   valid pointer to index, valid index filename
 * We guarantee:
 *   a NULL index is ignored; a file that cannot be written is ignored
 *   print each word in a new line with the docID, count pair
 * Note:
 *   index_save uses helper functions that help print the output 
 */
void index_save(index_t* idx, char* indexFilename);


/***************** index_load()  *******************/
/* iterates through created indexfile to write to a new indexfile
 * Call provides:
 *   valid pointer to indexfile that has been created by indexer
 * We return:
 *   an index with each word in a new line with the docID, count pair
 *   NULL if indexfile cannot be read 
 * Note:
 *   order may be different from the old index file 
 *   we assume indexfile is created in format detailed in spec
 */
index_t* indexLoad(char* indexFilename);



#endif // __INDEX_H
