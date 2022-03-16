/*
 * indextest.c - CS50 'indexer' module
 * 
 * The indexer is a program that test  document files created by indexer 
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

/****************** local functions ******************/
/* local function prototypes */
static void parseArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename);

/************************** main **************************/
/* Main function calls parseArgs and indexBuild
 * 
 * We assume:
 *   index file follow the form specified in requirement spec
 * We return:
 *   0 upon successful parseArgs and indexBuild
 * We guarantee:
 *   creation of index file from another index file
 * Caller is responsible for:
 *   
 * Notes:
 *   used to validate a file created by indexer
 */
int
main(const int argc, char* argv[])
{
  char* oldIndexFilename = NULL;
  char* newIndexFilename = NULL;
  
  // parse command line, validate parameters
  parseArgs(argc, argv, &oldIndexFilename, &newIndexFilename);
  
  index_t* index;
  index = indexLoad(oldIndexFilename);  

// write to file and delete index
  index_save(index, newIndexFilename);
  index_delete(index);


  exit(0);  
}

/* ************************ parseArgs ***************************** */
/* parseArgs parses the command line arguments
 * 
 * We return:
 *   nonzero if incorrect usage
 *   nonzero if invalid pageDirectory or indexFilename
 * We guarantee:
 *   creation of new indexfile given valid indexfile
 * Caller is responsible for:
 *   providing valid files for reading and writing
 * Notes:
 *   
 */
static void
parseArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename)
{
#ifdef DEBUG
  printf("Start of parseArgs");
#endif
  
  // error message for incorrect usage
  if (argc!=3){
    fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename\n", argv[0]);
    exit(1);
  }
  
  // store command line arguments as parameters
  *oldIndexFilename = argv[1];
  *newIndexFilename = argv[2];  

  // check if seedURL or pageDirectory is NULL
  if (*oldIndexFilename == NULL || *newIndexFilename == NULL){
    fprintf(stderr, "parseArgs: invalid oldIndexFilename or newIndexFilename\n");
    exit(1);
  }
  
  FILE* fp1 = NULL;
  if ((fp1 = fopen(*oldIndexFilename, "r")) == NULL) {
    fprintf(stderr, "indextest: cannot open %s", *oldIndexFilename);
    exit(1);
  }
  fclose(fp1);

  FILE* fp2 = NULL;
  if ((fp2 = fopen(*newIndexFilename, "w")) == NULL) {
    fprintf(stderr, "indextest: cannot write file: %s", *newIndexFilename);
    exit(1);
  }
  fclose(fp2); 
  
#ifdef MEMTEST
  mem_report(stdout, "After paraseArgs()");
#endif

  return;
}
