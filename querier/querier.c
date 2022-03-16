/*
 * querier.c - CS50 'querier' module
 * 
 * The querier is a  program that takes a query and return matching 
 * documents in ranked order. It handles queries that are either joined 
 * by 'and' or 'or'. Two words without any conjunction is considered an
 *  implicit 'and'.
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab6
 * May, 2021 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "hashtable.h"
#include "webpage.h"
#include "pagedir.h"
#include "mem.h"
#include "index.h"
#include "word.h"
#include "file.h"
#include "counters.h"
#include "bag.h"
/**************** local data structure *******************/

/**************** wordlist struct  ***********/
// contains an array of words and number of words
typedef struct wordlist {
  char** list;
  int nwords;
} wordlist_t; 

/**************** wordlist_new ****************/
/* Create a new (empty) bag.
 *
 * We return:
 *   a pointer to wordlist struct for given words
 *   NULL if there are zero or less words 
 *   NULL if memory allocation error 
 * Caller is responsible for:
 *   later calling wordlist_delete
 */

static wordlist_t*
wordlist_new(char** words, int size) {
  if (size <=0) {
    return NULL;
  }
  wordlist_t* wordlist = mem_malloc(sizeof(words)+sizeof(int)+1);
  wordlist->list = words;
  wordlist->nwords = size;

  return wordlist;
}

/**************** wordlist_delete ****************/
/* Delete the array of words.
 *
 * Caller provides:
 *   a valid wordlist struct
 * We guarantee:
 *   to free memory allocated for each array item
 *   to free the array and struct 
 */

static void
wordlist_delete(wordlist_t* wordlist){

  if (wordlist==NULL) {
    return;
  }
  for (int i = 0; i < wordlist->nwords; i++) {
    // free malloc'd words
    mem_free(wordlist->list[i]);    
  }
  mem_free(wordlist->list);
  mem_free(wordlist);
}

/**************** intersectArgs struct  ***********/
// contains two counters for intersect and result
struct intersectArgs {
  counters_t* setA;
  counters_t* setB;
};

/**************** rankitem struct  ***********/
// contains ID and count for sorting doc by score
typedef struct rankitem {
  int ID;
  int count;
}rankitem_t;

/****************** local functions ******************/
static void prompt(void);
int fileno(FILE *stream);

static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static wordlist_t* tokenize (char* line);

static bool andCheck (char* word);
static bool orCheck (char* word);

static bool validateQuery(index_t* index, wordlist_t* wordlist);
static void cleanQuery (wordlist_t* wordlist);

static counters_t* query(index_t* index, wordlist_t* wordlist);
static counters_t* andSequence(index_t* index, char** temp, int nwords);
static void rankResult(counters_t* result, char* pageDirectory);

/* helper functions */
static void counters_copy_helper(void* arg, const int key, const int count);
static void counters_and_helper(void* arg, const int key, const int count);
static void counters_or_helper(void* arg, const int key, const int count);

static void itemcount(void* arg, const int key, const int count);
static void itemdelete(void* item);

static void insertStruct (void* arg, const int key, const int count);
static int qsort_helper (const void* itemA, const void* itemB);

/******************************* main ************************************/
/* Main function calls: 
 *   parseArgs, indexload, tokenize, validateQuery, cleanQuery,
 *   query, rankResult, index_delete
 * We assume:
 *   caller provides valid pageDirectory and indexFilename
 * We return:
 *   0 when the user enters control-D or EOF is read 
 *   nonzero if any arguments provided are invalid 
 * We guarantee:
 *   a list of doc ID and URL for matching queries ranked by score 
 * Caller is responsible for:
 *   providing pageDirectory from crawler and indexfile from indexer
 *   valid query as specified by the specs
 * Notes:
 *   exit by entering control-D
 */
int
main(const int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  // parse command line, validate parameters
  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  // load index 
  index_t* index = NULL;   
  index = indexLoad(indexFilename);
  
  char* line;
  //FILE* fp = stdin;
  counters_t* result;              // contains result of query

  prompt();
  // reads until EOF (control-D to quit)
  while ((line = file_readLine(stdin)) != NULL) {

    // ignores enter key
    if (strcmp(line,"")!=0) {

      // parses a line into an array of words
      wordlist_t* wordlist = tokenize(line);

      if (wordlist != NULL) {
        // check for invalid query 
        if (validateQuery(index, wordlist)){
          cleanQuery(wordlist);         // prints clean query
        
        // process query and rank results 
          result = query(index, wordlist);
          rankResult(result, pageDirectory);
          
          fprintf(stdout, "-----------------------------------------------\n");
        } else {
          wordlist_delete(wordlist);
        }
      }
    }
    free(line);
    prompt();
  }
  fprintf(stdout,"\n");
  
  // close index
  index_delete(index);

  exit(0); 
}

/* ******************* rankResult ************************** */
/* Puts counternodes into struct for sorting
 * 
 * We assume:
 *   caller provides counter after all and/or sequences
 *   valid pagedirectory from crawler
 * We return:
 *   print out message if no documents match
 *   print out documents ranked in descending order
 * We guarantee:
 *   output of query is printed out as per spec 
 * Notes:
 *   
 */
static void
rankResult(counters_t* result, char* pageDirectory)
{
#ifdef DEBUG
  printf("start of rankResult");
#endif

  int ncount = 0;
  counters_iterate(result, &ncount, itemcount); 

  if (ncount == 0) {
    fprintf(stdout, "No documents match\n");
    counters_delete(result);
    return;
  } 
  rankitem_t** ranked = mem_calloc_assert(ncount, sizeof(rankitem_t*)+1, "out of memory");

  // first insert into bag
  bag_t* structs = bag_new();
  counters_iterate(result, &structs, insertStruct);
  counters_delete(result);
  
  // pop and sort
  for (int i = 0; i < ncount; i++) {
    ranked[i] = bag_extract(structs);
  }
  bag_delete(structs, itemdelete);
  qsort(ranked, ncount, sizeof(rankitem_t*), qsort_helper);
  
  fprintf(stdout, "Matches %d documents (ranked) :\n", ncount);
  for (int i = 0; i < ncount; i++) {

    //load a page from file in that direcotry
    char* pathname = NULL;
    pathname = mem_malloc_assert(strlen(pageDirectory)+ strlen("/")+ sizeof(ranked[i]->ID)+1, "out of memory");
    sprintf(pathname, "%s/%d", pageDirectory, ranked[i]->ID);

    FILE* fp = NULL;
    if ((fp = fopen(pathname, "r")) == NULL) {
      mem_free(pathname);
      return;
    } 

    // first line contains URL
    char* url = NULL;
    url = file_readLine(fp);
    fprintf(stdout, "score  %2d doc %3d:  %s \n", ranked[i]->count, ranked[i]->ID, url);

    // free rankitem struct
    mem_free(ranked[i]);
    free(url);
    fclose(fp);
    mem_free(pathname);
  }
  mem_free(ranked);

#ifdef MEMTEST
  mem_report(stdout, "After rankResult");
#endif

}

/* ******************* query ************************** */
/* combines counters depending on 'and'/'or' sequence
 * 
 * We assume:
 *   caller provides valid index and parsed wordlist
 * We return:
 *   NULL if there is a memory allocation error
 *   a counters the contains the result of the query
 * We guarantee:
 *   that 'and' takes precednece over ors
 * Caller is responsible for:
 *   
 * Notes:
 *   
 */
static counters_t*
query(index_t* index, wordlist_t* wordlist)
{
#ifdef DEBUG
  printf("start of query");
#endif
  
  char** words = wordlist->list;
  int nwords = wordlist->nwords;
  // holds words to be passed to andSequence
  char** temp = mem_calloc_assert(nwords, sizeof(words)+1, "out of memory");
  
  // contains result from each and sequence 
  bag_t* bigresult = mem_assert(bag_new(), "out of memory");
  counters_t* result = mem_assert(counters_new(), "out of memory");
  int count = 0; 
  
  for (int i = 0; i < nwords; i++) {
  
    if (orCheck(words[i])) {    
      
      // call andSequence with words in temp
      counters_t* miniresult;
      if((miniresult = andSequence(index, temp, count))!= NULL){
        bag_insert(bigresult, miniresult);
        
        free(temp);
        temp = mem_calloc_assert(nwords, sizeof(words)+1, "out of memory");  
        count = 0;

      };
      
    } else {
      // skip over 'and'
      if (andCheck(words[i])) {
        continue;
      }
      // put andSequences into temp array
      temp[count] = words[i];
      count++;
    }
  }
 
  // last group of words after or
  counters_t* lastresult;
  if ((lastresult = andSequence(index, temp, count)) != NULL) {
    bag_insert(bigresult, lastresult);
  }
  free(temp);
  
  // sum up all and sequences (miniresults)
  counters_t* cntr = NULL;
  struct intersectArgs args = {NULL, result};

  while ((cntr = bag_extract(bigresult)) != NULL) {
    counters_iterate(cntr, &args, counters_or_helper);
    counters_delete(cntr);
    
  }
  bag_delete(bigresult, itemdelete);
  wordlist_delete(wordlist);

#ifdef MEMTEST
  mem_report(stdout, "After query");
#endif
  
  // return the final counters
  return args.setB;
}

/* ******************* andSequence ************************** */
/* function that process a series of 'and' queries
 *
 * We assume:
 *   caller provides a valid index and parsed array of words
 * We return:
 *   a counters that contains the result of 'and' sequences
 *   NULL if we fail to allocate memeory
 * We guarantee:
 *  the intersection of all counters of the 'and' sequence 
 * Caller is responsible for:
 *   freeing the counter that is returned
 * Notes:
 *   implict and explicit ands have equal precedence
 */
static counters_t*
andSequence(index_t* index, char** temp, int count)
{
#ifdef DEBUG
  printf("start of andSequence");
#endif

  counters_t* setA = mem_assert(counters_new(), "out of memory");
  struct intersectArgs args;
  counters_t* count1;
  counters_t* count2;
  bool first = true;
  for (int i = 0; i < count; ) {
    if (count == 1) {
      counters_t* cntrs;
      if ((cntrs = index_find(index, temp[i])) != NULL) {
        counters_iterate(cntrs, setA, counters_copy_helper);
        return setA;
      } else {

        return setA;
      }
    }
    if (first) {
      count1 = index_find(index, temp[i]);
      counters_iterate(count1, setA, counters_copy_helper);
      first=false;
      i++; 
    }

    if ((count2 = index_find(index, temp[i])) != NULL) {      
      args.setA = setA;
      args.setB = count2;
      counters_iterate(count1, &args, counters_and_helper);
    }
    i++;

  }
  
#ifdef MEMTEST
  mem_report(stdout, "After andSequence");
#endif
  return setA;
  
}

/* *************** counters_and_helper  ********************** */
/* helper function that inputs the intersection into a third counter
 *
 * We assume:
 *   caller provides two counters to intersect and insert the result
 * We guarantee:
 *   counters being intersected remain unchanged
 * Notes:
 *   Reference: CS50 Lecture Notes - Iterators
 */
static void 
counters_and_helper(void* arg, const int key, const int count)
{

  counters_t* countA = ((struct intersectArgs*)arg)->setA;
  counters_t* countB = ((struct intersectArgs*)arg)->setB;

  int countA_count = count;

  // look for key in count2
  int countB_count = counters_get(countB, key);
  
  if (countB_count == 0) { 
    counters_set(countA, key, 0);
  } else {
    // set to the lower count
    if (countB_count > countA_count) {
      counters_set(countA, key, countA_count);
    } else {
      counters_set(countA, key, countB_count);
    }
  }
 
}
/* **************** counters_or_helper  *********************** */
/* a helper function that inputs the union into a third counter
 *
 * We assume:
 *   caller provides two counters to merge and insert the result
 * We guarantee:
 *   counters being merged remain unchanged
 * Notes:
 *   Reference: CS50 Lecture Notes - Iterators
 */
static void 
counters_or_helper(void* arg, const int key, const int count)
{
  counters_t* result = ((struct intersectArgs*)arg)->setB;
  int count1 = count;
  
  // find the same key in countersA
  int count_result = counters_get(result, key);
  if (count_result == 0) {
    // not found: insert it
    counters_set(result, key, count1);

  } else {
    // add to the existing value
    count_result += count1;
    counters_set(result, key, count_result);    
  }
    
}

/* ******************* validateQuery ************************** */
/* checks the words parsed for invalid queries
 *
 * We assume:
 *   caller provides a valid index and array of words parsed
 * We return:
 *   true if the query is valid
 *   false if the query is invalid
 * We guarantee:
 *   queries are only valid if conditions in spec are met
 */
static bool
validateQuery(index_t* index, wordlist_t* wordlist)
{
#ifdef DEBUG
  printf("start of validateQuery");
#endif

  char** words = wordlist->list;
  int nwords = wordlist->nwords;
  
  for (int j = 0; j < nwords; j++) {

    // check if queries begin with and/or
    if (andCheck(words[0])) {
      fprintf(stderr, "Error: 'and' cannot be first\n");
      return false;
    }
    if (orCheck(words[0])) {
      fprintf(stderr, "Error: 'or' cannot be first\n");
      return false;
    }
    // check if queries end with and/or
    if (andCheck(words[nwords-1])) {
      fprintf(stderr, "Error: 'and' cannot be last\n");
      return false;
    }
    // check if queries end with and/or
    if (orCheck(words[nwords-1])) {
      fprintf(stderr, "Error: 'or' cannot be last\n");
      return false;
    }
    
    // after first and before last word
    if (j > 0 && j < nwords-2) {

      // check for two consecutive ands
      if (andCheck(words[j]) && andCheck(words[j+1])) { 
        fprintf(stderr, "'and' and 'and' cannot be adjacent\n");
        return false;
      }

      // check for two consecutive ors
      if (orCheck(words[j]) && orCheck(words[j+1])) { 
        fprintf(stderr, "'or' and 'or' cannot be adjacent\n");
        return false;
      }

      // check for adjacent and and or
      if (andCheck(words[j]) && orCheck(words[j+1])) { 
        fprintf(stderr, "'and' and 'or' cannot be adjacent\n");
        return false;
      }

      // check for adjacent or and and
      if (orCheck(words[j]) && andCheck(words[j+1])) { 
        fprintf(stderr, "'or' and 'and' cannot be adjacent\n");
        return false;
      }
      
    }
  }

#ifdef MEMTEST
  mem_report(stdout, "After validateQuery");
#endif

  return true;
}

/* ******************* andCheck ************************** */
/* a simple function that checks if the word is an 'and'
 *
 * We assume:
 *   caller provides a valid word
 * We return:
 *   true if the word is an 'and'
 *   false if the word is not an 'and'
 */
static bool
andCheck (char* word) {
  if (word==NULL) {
    return NULL;
  }
  if (strcmp(word, "and")==0) {
    return true;
  } else {
    return false;
  }
}
/* ******************* orCheck ************************** */
/* a simple function that checks if the word is an 'or'
 *
 * We assume:
 *   caller provides a valid word
 * We return:
 *   true if the word is an 'or'
 *   false if the word is not an 'or'
 */
static bool
orCheck (char* word) {
  if (word==NULL) {
    return NULL;
  }
  if (strcmp(word, "or")==0) {
    return true;
  } else {
    return false;
  }
}

/**************** cleanQuery ****************/
/* Prints out a clean query that has been normalized and validated
 *
 * Caller provides:
 *   valid wordlist struct that contains parsed queries
 * We return:
 *   print out a clean query in a new line 
 * Caller is responsible for:
 *   Nothing 
 */
static void
cleanQuery (wordlist_t* wordlist) {
  if (wordlist == NULL) {
    return;
  }
  fprintf(stdout, "Query: ");
  for (int j = 0; j < wordlist->nwords; j++) {
    fprintf(stdout, "%s ",  wordlist->list[j]);
  }
  fprintf(stdout, "\n");
}


/**************** tokenize ****************/
/* parses a line in an array of strings
 *
 * Caller provides:
 *   valid char pointer to a line
 * We return:
 *   NULL if the length of the line is zero
 *   NULl if there is a nonletter character 
 *   return a wordlist struct with array and array size 
 * We guarantee:
 *   each word is normalized and checked for invalid characters
 * Caller is responsible for:
 *   free-ing the array in the struct returned
 * Note:
 *   
 */
static wordlist_t*
tokenize (char* line)
{
#ifdef DEBUG
  printf("start of tokenize");
#endif
  
  const int lineLength = strlen(line); // len of line
  // ignore empty spaces
  if (lineLength==0) {
    return NULL;
  }
  // create a list of words 
  char** words = calloc((lineLength+1)/2, sizeof(char*));
  char* word = NULL;
  int n = 0;              // counts number of words 

  for (int i = 0; i < lineLength; ) {
    // move along until first non-space
    while (isspace(line[i])) {
      i++;
    }
    // print error and exit if nonletter char
    if (!isspace(line[i]) && !isalpha(line[i]) && line[i]!='\0') {
      fprintf(stdout, "Error1: bad character '%c' in query\n", line[i]);
      return NULL;
    }
    // beginning of word
    if (isalpha(line[i])) {
      word = &line[i];  
      i++;
      
      while (isalpha(line[i])) {
        i++;
      }
      // print error and exit if nonletter within word
      if (!isspace(line[i]) && !isalpha(line[i]) && line[i]!='\0') {
        fprintf(stdout, "Error2: bad character '%c' in query\n", line[i]);
        return NULL;
      }
      
      // terminate with null to create word 
      line[i] = '\0';
    
      // insert normalized word into list
      char* norm_word = normalizeWord(word);
      words[n] = norm_word;
      
      n++;
    }
    i++;
  }
  if (n > 0) {
    wordlist_t* wordlist = wordlist_new(words, n);
    return wordlist;
  } else {
    return NULL;
  }

#ifdef MEMTEST
  mem_report(stdout, "After tokenize");
#endif

}

/* ************************ parseArgs ***************************** */
/* parseArgs parses the command line arguments
 * 
 * We return:
 *   nonzero if incorrect usage
 *   nonzero if invalid pageDirectory or indexFilename
 * We guarantee:
 *   pageDirectory has been created by crawler with at least one file
 *   index file can be read 
 * Caller is responsible for:
 *   providing pageDirectory from crawler indexfile from indexer
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
  
  // check if pageDirectory or indexFilename is NULL
  if (*pageDirectory == NULL || *indexFilename == NULL){
    fprintf(stderr, "parseArgs: invalid pageDirectory or indexFilename\n");
    exit(2);
  }

  // check if pageDirectory is readable
  FILE* fp = NULL;
  if ((fp = fopen(*pageDirectory, "r")) == NULL){
    fprintf(stderr, "parseArgs: cannot open directory %s\n", *pageDirectory);
    exit(3);
  }
  fclose(fp);
  
  // check if pagedir created bt crawler
  if (!pagedir_validate(*pageDirectory)){
    fprintf(stderr, "parseArgs: not a file created by crawler\n");
    exit(4);
  }

  // check if at least one file exists
  webpage_t* webpage;
  if ((webpage = pagedir_load(*pageDirectory, 1)) == NULL) {
    fprintf(stderr, "parseArgs: there is no file in directory");
    exit(5);
  }
  webpage_delete(webpage);
  
  // check if can read indexFilename
  FILE* fp1 = NULL;
  if ((fp1 = fopen(*indexFilename, "r")) == NULL){
    fprintf(stderr, "parseArgs: cannot read %s\n", *indexFilename);
    exit(6);
  }
  fclose(fp1);

#ifdef MEMTEST
  mem_report(stdout, "After parseArgs");
#endif

  return;
}

/* ******************* prompt ************************** */
/* prompts the user for a query
 *
 * We return:
 *   print the word 'Query?'
 * We guarantee:
 *   to prompt if the stdin is a termianl  
 * Notes:
 *   Reference: CS50 TSE Querier README.md
 */
static void
prompt(void)
{
  //tty means terminal
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}

/************************* helper functions ******************************/

/* ******************* itemdelete ************************** */
/* a helper function for a bag that contains counters item
 * 
 * We return:
 *   Nothing 
 * We guarantee:
 *   To free all items that are not NULL
 */
static void
itemdelete(void* item)
{
  if (item != NULL) {
    counters_delete(item);   
  }
}

/* ******************* itemcount ************************** */
/* helper function that counts number of itmes in counters
 *
 * We assume:
 *   caller provides a valid counter and integer for counting
 * We return:
 *   Nothing 
 * We guarantee:
 *   to count all items with valid key and count
 */
static void
itemcount(void* arg, const int key, const int count)
{
  int* ncounts = arg;
  if (ncounts != NULL && count!=0) {
    (*ncounts)++;
  }
}

/* ******************* counters_copy_helper ************************** */
/* a simple function that copies one counter to another  
 *
 * We assume:
 *   caller an empty counter as args
 * We return:
 *   Nothing 
 * We guarantee:
 *   all counters item are copied to counters in args
 */
static void 
counters_copy_helper(void* arg, const int key, const int count)
{
  counters_set(arg, key, count);
}

/* ******************* insertStruct ************************** */
/* takes item from a counters and puts them into rankitem struct
 * 
 * We assume:
 *   call provides a data structure that to contain struct
 * We return:
 *   NULL if the data structure is empty
 *   a bag of rankitem structs
 * We guarantee:
 *   each rankitem has a key and count
 * Caller is responsible for:
 *   freeing the malloc'd rankitem_t items
 * Notes:
 *   
 */
static void
insertStruct (void* arg, const int key, const int count)
{
  // to contain the structs
  bag_t** structs = arg;
  if (structs != NULL&&count!=0){
    
    rankitem_t* item = malloc(sizeof(rankitem_t*));
    item->ID=key;
    item->count=count;
    bag_insert((*structs), item);
  }
  
}

/* ******************* qsort_helper  ************************** */
/* a helper function utilized by built-in qsort
 * 
 * We assume:
 *   caller provides an array of rankitem_t
 * We return:
 *   0 if the scores are equivalent
 *   >0 if score 2 is greater than score 1
 *   <0 if score 1 is greater than score 2
 * We guarantee:
 *   array is sorted in descending order 
 * Caller is responsible for:
 *   passing the helper function in qsort
 */
static int
qsort_helper (const void* itemA, const void* itemB)
{
  rankitem_t* item1 = *(rankitem_t**)itemA;
  rankitem_t* item2 = *(rankitem_t**)itemB;
  int score1 = item1->count;
  int score2 = item2->count;
  
  return (score2>score1)-(score2<score1);
}
