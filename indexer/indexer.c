/*
 * indexer.c - CS50 'indexer' module
 * 
 * The indexer is a standalone program that reads document files created by TSE 
 * crawler to build and write an index. indextest.c can be used to read this 
 * index and save it to another file.
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab5
 * May, 2021 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "webpage.h"
#include "pagedir.h"
#include "mem.h"
#include "index.h"
#include "word.h"

/****************** local functions ******************/
/* local function prototypes */

static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void indexBuild (char* pageDirectory, char* indexFilename);
static void indexPage (webpage_t* webpage, int docID, index_t** idx);

/************************** main **************************/
/* Main function calls parseArgs and indexBuild
 * 
 * We assume:
 *   caller provides valid pageDirectory and indexFilename
 * We return:
 *   0 upon successful parseArgs and indexBuild
 * We guarantee:
 *   creation of index file for valid directory and output file
 * Caller is responsible for:
 *   providing pageDirectory created by crawler and writable file
 * Notes:
 * 
 */
int
main(const int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  
  // parse command line, validate parameters
  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  
  // call indexBuild with pageDirectory
  indexBuild(pageDirectory, indexFilename);
  
  exit(0); 
}

/* ************************ parseArgs ***************************** */
/* parseArgs parses the command line arguments
 * 
 * We return:
 *   nonzero if incorrect usage
 *   nonzero if invalid pageDirectory or indexFilename
 * We guarantee:
 *   creation of indexfile given valid argumetns
 * Caller is responsible for:
 *   providing pageDirectory created by crawler
 * Notes:
 *   
 */
static void
parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
#ifdef DEBUG
  printf("start of parseArgs");
#endif

  // error message for incorrect usage
  if (argc!=3){
    fprintf(stderr, "Usage: %s pageDirectory indexFilename\n", argv[0]);
    exit(1);
  }
  
  // store command line arguments as parameters
  *pageDirectory = argv[1];
  *indexFilename = argv[2];
  
  // check if seedURL or pageDirectory is NULL
  if (*pageDirectory == NULL || *indexFilename == NULL){
    fprintf(stderr, "parseArgs: invalid pageDirectory or indexFilename\n");
    exit(1);
  }

  // check if pageDirectory is readable
  FILE* fp1 = NULL;
  if ((fp1 = fopen(*pageDirectory, "r")) == NULL){
    fprintf(stderr, "parseArgs: cannot open directory %s\n", *pageDirectory);
    exit(1);
  }
  fclose(fp1);

  // call pagedir_init()
  if (!pagedir_validate(*pageDirectory)){
    fprintf(stderr, "parseArgs: not a file created by crawler\n");
    exit(1);
  }

  // check if at least one file exists
  webpage_t* webpage;
  if ((webpage = pagedir_load(*pageDirectory, 1)) == NULL) {
    fprintf(stderr, "parseArgs: there is no file in directory");
    exit(1);
  }
  webpage_delete(webpage);
  
  // check if can write indexFilename
  FILE* fp2 = NULL;
  if ((fp2 = fopen(*indexFilename, "w")) == NULL){
    fprintf(stderr, "parseArgs: cannot open %s\n", *indexFilename);
    exit(1);
  }
  fclose(fp2);

#ifdef MEMTEST
  mem_report(stdout, "After parseArgs");
#endif
  
  return;
}

/* ************************ indexBuild ***************************** */
/* Build an in-memory index from pageDirectory and saves to indexFilename
 * We assume:
 *   that index size of 200 is sufficient
 * We return:
 *   nothing if the index is successfully created
 *   non-zero if failed to create index
 * We guarantee:
 *   a docID, count pair for each unique word in the webpage  
 * Caller is responsible for:
 *   
 * Notes:
 *   indexPage is called to scan for words to add
 */
static void
indexBuild (char* pageDirectory, char* indexFilename)
{
#ifdef DEBUG
  printf("start of indexBuild");
#endif

  index_t* index = mem_assert(index_new(200), "cannot create index");
  int docID = 1;
  webpage_t* webpage = NULL; 
  
  // loop over document ID numbers, count from 1
  while ((webpage = pagedir_load(pageDirectory, docID)) != NULL) {

#ifdef DEBUG
    printf("\ncurrent docID: %d URL: %s", docID, webpage_getURL(webpage));
#endif 

    // if successful, pass the webpage and docID to indexPage    
    indexPage(webpage, docID, &index);
    // webpage delete
    webpage_delete(webpage);
    docID++; 
  }
  // write the indexFile
  index_save(index, indexFilename);
  index_delete(index);

#ifdef MEMTEST
  mem_report(stdout, "After indexBuild");
#endif

}

/* ************************ indexPage ***************************** */
/* scans a webpage document to add its words to the index.
 * We assume:
 *   caller provides valid webpage, docID > 1 and index 
 * We return:
 *   nothing
 * We guarantee:
 *   new words are added to the index with a counter
 *   increment of count for docID if word already in index
 * Caller is responsible for:
 *    
 * Notes:
 *   normalizeWord converts a word into lowercase
 */
//scans a webpage document to add its words to the index.
static void
indexPage (webpage_t* webpage, int docID, index_t** idx)
{
#ifdef DEBUG
  printf("start of indexPage");
#endif

  //steps through each word of the webpage
  char* word = NULL;
  int pos = 0;
  
  while ((word = webpage_getNextWord(webpage, &pos)) != NULL) {

    //skips trivial words 
    if (strlen(word) > 2) {

      //normalizes the word 
      char* normWord = normalizeWord(word);
      counters_t* ctrs = NULL;

      //looks up word in index
      if ((ctrs = index_find(*idx, normWord)) == NULL) {

        //adding the word to the index if necessary
        counters_t* counter = mem_assert(counters_new(), "cannot create counters");
        counters_add(counter, docID);
        index_insert(*idx, normWord, counter);
        
      } else {
        //increments the count of word for this doc
        counters_add(ctrs, docID);
      }
      mem_free(normWord);
    }
    free(word); 
  }
#ifdef MEMTEST
  mem_report(stdout, "After indexPage");
#endif
}
