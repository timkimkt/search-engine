/*
 * crawler.c
 * 
 * see crawler.h for more information
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab4
 * April, 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "hashtable.h"
#include "webpage.h"
#include "pagedir.h"
#include "mem.h"

/****************** local functions ******************/
/* local function prototypes */

static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void logr(const char *word, const int depth, const char *url);

/************************** main **************************/
/* Main function calls parseArgs and crawl
 * 
 * We assume:
 *   caller provides valid seedURL, pageDirectory, maxDepth
 *   pageDirectory does contain any files whose name is an integer
 * We return:
 *   0 upon successful parseArgs and crawl
 * We guarantee:
 *   creation of pageDirectory files for valid URL and depth  
 * Caller is responsible for:
 *   creating pageDirectory folder in advance
 * Notes:
 * 
 */
int
main(const int argc, char* argv[])
{
  // error message for incorrect usage
  if (argc!=4){
    fprintf(stderr, "Usage: %s seedURL pageDirectory maxDepth\n", argv[0]);
    exit(1);
  }

  // initialize function parameters
  char* seedURL = NULL;
  char* pageDirectory = NULL;
  int maxDepth = 0;
  
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

#ifdef DEBUG
  printf("\nParsed: %s, %s, %d\n", seedURL, pageDirectory, maxDepth);
#endif

#ifdef MEMTEST
  mem_report(stdout, "After parseArgs");
#endif
  
  crawl(seedURL, pageDirectory, maxDepth);

#ifdef MEMTEST
  mem_report(stdout, "At end of Main");
  printf("\nNet malloc-free counts (should be zero): %d", mem_net());
#endif
  
  exit(0);
}

/****************** parseArgs() ******************/
/* parseArgs parses the command line arguments
 * 
 * We return:
 *   nonzero if invalid seedURL, pageDirectory, maxDepth
 * We guarantee:
 *   creation of .crawler file to mark the directory
 * Caller is responsible for:
 *   creation of pageDirectory folder
 * Notes:
 *   use of normURL requires mem_free
 */
static void
parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
#ifdef DEBUG
  printf("start of parseArgs");
#endif

  // store command line arguments as parameters
  *seedURL = argv[1];
  *pageDirectory = argv[2];
  
  char* nextchar = NULL;
  if (sscanf(argv[3], "%d%c", maxDepth, nextchar) != 1){
    fprintf(stderr, "parseArgs: maxDepth is not a valid integer\n");
    exit(1);
  }

  // check if maxDepth is within range
  if (*maxDepth < 0 || *maxDepth > 10){
    fprintf(stderr, "parseArgs: maxDepth out of range\n"); 
    exit(1);
  }

  // check if seedURL or pageDirectory is NULL
  if (*seedURL == NULL || *pageDirectory == NULL){
    fprintf(stderr, "parseArgs: seedURL or pageDirectory NULL\n");
    exit(1);
  }
  // normalize the URL
  char* normURL = normalizeURL(*seedURL);

  // check if the URL is internal
  if (!isInternalURL(normURL)) {
    fprintf(stderr, "parseArgs: seedURL is not internal\n");
    exit(1);
  }
  // call pagedir_init()
  if (!pagedir_init(*pageDirectory)){
    fprintf(stderr, "parseArgs: unable to write pageDirectory/.crawler\n");
    exit(1);
  };
  
  // normURL uses malloc
  free(normURL);
  //mem_free(pageDirectory);

#ifdef DEBUG
  printf("end of parseArgs");
#endif

  return;
}

/****************** crawl() ******************/
/* crawls webpages starting from seedURL until maxDepth
 * 
 * We assume:
 *   caller provides valid seedURL, pageDirectory and maxDepth
 * We return:
 *   nonzero if invalid parameters or fail to allocate memory
 * We guarantee:
 *   crawl and save valid internal URL until maxDepth
 * Notes:
 *   hashtable is intialized with a slot of 200 
 *   the item of hashtable is a constant empty string
 *   docID starts from 1
 */
static void
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  
#ifdef DEBUG
  printf("start of crawl: seedURL: %s, pageDir: %s, maxDepth: %d", seedURL, pageDirectory, maxDepth);
#endif
  
  if (seedURL == NULL || pageDirectory == NULL || maxDepth < 0) {
    fprintf(stderr, "crawl: invalid crawl parameters");
    exit(1);
  }

  // copy URL to add to webpage
  // malloc  b/c webpage doesn't use mem module
  char* copyURL = NULL;
  copyURL = malloc(strlen(seedURL)+1);
  if (copyURL == NULL) {
    fprintf(stderr, "failed to copy URL\n");
    exit (99);
  }
  strcpy(copyURL, seedURL);

  // initialize hashtable and insert seedURL
  hashtable_t* pgSeen = mem_assert(hashtable_new(200), "failed to create hashtable");
  hashtable_insert(pgSeen, seedURL, "");

   // initialize bag and add webpage with seedURL, marked with depth = 0
  int depth = 0;
  bag_t* pgToCrawl = mem_assert(bag_new(), "failed to create bag\n");

  // insert webpage to bag
  webpage_t* webpage = webpage_new(copyURL, depth, NULL);
  // manual check b/c webpage doesn't use mem module
  if (webpage == NULL) {
    fprintf(stderr, "failed to create webpage\n");
    exit(99);
  }  
  bag_insert(pgToCrawl, webpage);
  
  // while bag is not empty, extract item 
  int docID = 0;
  webpage_t* webpg = NULL;
  while ((webpg = bag_extract(pgToCrawl)) != NULL) {

    // retrieve webpage for URL and if fetch succesful
    if(webpage_fetch(webpg)) {
      
      // LOG FETCHED
      logr("Fetched", webpage_getDepth(webpg), webpage_getURL(webpg));

      // save webpage to page directory with unique ID
      pagedir_save(webpg, pageDirectory, ++docID);

      // if webpage is not at maxDepth explore to find links
      if (webpage_getDepth(webpg) < maxDepth) {
        pageScan(webpg, pgToCrawl, pgSeen); 
      }
    }
    // delete webpage
    webpage_delete(webpg);
  }

  // delete hashtable and bag
  hashtable_delete(pgSeen, NULL);
  bag_delete(pgToCrawl, webpage_delete);

#ifdef MEMTEST
  mem_report(stdout, "After crawl");
#endif

#ifdef DEBUG
  printf("end of crawl");
#endif

}

/****************** pageScan() ******************/
/* pageScan scans given page to extract URLs
 * 
 * We assume:
 *   caller provides valid page, bag and hashtable
 * We return:
 *   nonzero if fail to allocate memory
 * We guarantee:
 *   to free all URL pointers 
  * Notes:
 *   We do not use mem module for webpage 
 */
static void
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {

#ifdef DEBUG
  printf("start of pageScan");
#endif
  
  // LOG SCANNING
  logr("Scanning", webpage_getDepth(page), webpage_getURL(page));
  
  // initialize parameters and current depth
  int pos = 0;
  char* url = NULL;
  int currDepth = 0;
  currDepth = webpage_getDepth(page)+1;

  // while there is another URL in page
  while ((url = webpage_getNextURL(page, &pos)) != NULL) {

    char* temp_url = url;                // store in temp to free
    
    // normalize the URL
    char* normURL = NULL;
    normURL = normalizeURL(url);
    char* temp_normURL = normURL;        // store in temp to free
    
    // if URL is internal
    if (isInternalURL(normURL)){

      // copy URL to sotre inside webpage 
      char* copyURL = NULL;
      copyURL = malloc(strlen(normURL)+1);
      strcpy(copyURL, normURL);
      
      // insert webpage into hashtable and if successful
      if (hashtable_insert(pagesSeen, normURL, " ")) {

        // create a webpage_t for it marked with depth + 1
        webpage_t* webpage = mem_assert(webpage_new(copyURL, currDepth, NULL), "failed to create  webpage");
        
        // insert webpage into the bag
        bag_insert(pagesToCrawl, webpage);
        
        //LOG ADDED
        logr("Added:", webpage_getDepth(webpage), webpage_getURL(webpage));

      } else {

        // LOG IGNDUPL
        logr("IgnDupl:", currDepth, copyURL);
        // free copyURL since not inserted to webpage
        free(copyURL);        // otherwise freed by webpage delete
       
      }
      // free normalized URL 
      free(temp_normURL);
    } else {
      // LOG IGNEXTRN
      logr("IgnExtrn:", currDepth, normURL);
      // free normailized URL
      free(normURL);
    }
    //free URL fetched by getNextURL
    free(temp_url);
  }
  
#ifdef MEMTEST
  mem_report(stdout, "After pagescan");
#endif
  
#ifdef DEBUG
  printf("end of pageScan");
#endif
  
}

/****************** logr() ******************/
/* Reference: CS50 Lecture Notes
 * Provides a log of one word (1-9 chars) about given URL
 * We assume:
 *   caller provides message, depth and URL
 * We return:
 *   print to stdout
 * Caller is responsible for:
 *   turning on APPTEST in the makefile
 * Notes:
 *   indents according to depth
 */
static void
logr(const char *word, const int depth, const char *url)
{
#ifdef APPTEST
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
#else
  ;
#endif
}
