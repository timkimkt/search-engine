/*
 * pagedir.c - CS50 'pagedir' module
 * 
 * see pagedir.h for more information
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab4,5
 * May, 2021 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pagedir.h"
#include "webpage.h"
#include "file.h"
#include "mem.h"

/****************** local functions  ******************/


/****************** pagedir_init()  ******************/
/* see pagedir.h for description */
bool
pagedir_init(const char* pageDirectory)
{
  // check if given pageDirectory is not NULL
  if (pageDirectory == NULL){
    fprintf(stderr, "pagedir_save: invalid pageDirectory");
    return false;
  }
  // construct pathname for .crawler in directory
  char* filename = ".crawler";
  FILE* fp = NULL;

  // create path to save file
  char* filepath = NULL;
  // allocate memory for filepath
  filepath = mem_malloc(strlen(filename)+strlen(pageDirectory)+strlen("/")+1);
  
  // copy and concatenate
  strcpy(filepath, pageDirectory);
  strcat(filepath, "/");
  strcat(filepath, filename);

  // if error in opening, return false (pathname)
  if ((fp = fopen(filepath, "w")) == NULL){
    fprintf(stderr, "pagedir_init: unable to write file");
    return false;
  }
  
  // close file and return true
  fclose(fp);
  
  //free path constructed
  mem_free(filepath);
  
#ifdef MEMTEST
  mem_report(stdout, "After pagedir_init");
#endif

  return true;
}

/****************** pagedir_save()  ******************/
/* see pagedir.h for description */

void
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
  if (page == NULL || pageDirectory == NULL || docID < 1){
    fprintf(stderr, "pagedir_save: invalid parameters");
    return;
  }

  // allocate memory and store ID as string  
  char* str_docID = NULL;
  str_docID = mem_malloc(sizeof(docID)+1);
  sprintf(str_docID, "%d", docID);
  char* temp_docID = str_docID;
  
  // make pathname to save files
  char* pathname = NULL;
  pathname = mem_malloc(strlen(pageDirectory)+strlen(str_docID)+strlen("/")+3);

  // copy and concatenate 
  strcpy(pathname, pageDirectory);
  strcat(pathname, "/");
  strcat(pathname, str_docID);
  
  FILE* fp = NULL;

  // if error in opening, return false
  if ((fp = fopen(pathname, "w")) == NULL){
    fprintf(stderr, "pagedir_save: unable to write file");
    return;
  }
  
  // print the URL
  fprintf(fp, "%s\n", webpage_getURL(page)); 
  // print the depth
  fprintf(fp, "%d\n", webpage_getDepth(page)); 
  // print the contents of the webpage
  fprintf(fp, "%s\n", webpage_getHTML(page)); 
    
  // close the file
  fclose(fp);

  mem_free(pathname);          // free pathname
  mem_free(temp_docID);        // free str_docID

#ifdef MEMTEST
  mem_report(stdout, "After pagedir_save()");
#endif
  
  return;

}

/****************** pagedir_validate()  ******************/
/* see pagedir.h for description */
bool
pagedir_validate (const char* pageDirectory) 
{ 

  // create path for ./crawler in directory
  char* pathname = NULL;
  pathname = mem_malloc(strlen("/.crawler") + strlen(pageDirectory)+1);
  sprintf(pathname, "%s/.crawler", pageDirectory);

  FILE* fp = NULL;
  // return false if ./crawler does not exist
  if ((fp = fopen(pathname, "r")) == NULL) {
    mem_free(pathname);
    return false;
  }
  
  // otherwise close file and return true 
  fclose(fp);
  mem_free(pathname);
  return true;

}

/****************** pagedir_load()  ******************/
/* see pagedir.h for description */
webpage_t*
pagedir_load (const char* pageDirectory, const int docID) {

  //load a page from file in that direcotry
  char* pathname = NULL;
  pathname = mem_malloc(strlen(pageDirectory)+ strlen("/")+ sizeof(docID)+1);
  sprintf(pathname, "%s/%d", pageDirectory, docID);

  FILE* fp = NULL;
  if ((fp = fopen(pathname, "r")) == NULL) {
    mem_free(pathname);
    return false;
  } 
  // first line contains URL
  char* url = NULL;
  url = file_readLine(fp);

  // second line contains depth
  int depth = 0;
  char* nextchar = NULL;
  char* depth_s = file_readLine(fp);
  if (sscanf(depth_s, "%d%c", &depth, nextchar) != 1){
    fprintf(stderr, "pagedir_load(): invalid depth");
  }

  // rest of file is HTML
  char* html = NULL;
  html = file_readFile(fp);

  // unlikely but check if webpage is valid
  if (url == NULL || depth < 0 || html == NULL) {
    free(depth_s);
    mem_free(pathname);
    return NULL;
  }

  // create a webpage
  webpage_t* webpage = NULL;
  webpage = webpage_new(url, depth, html);
  if (webpage == NULL) {
    fprintf(stderr, "pagedir_load(): failed to create webpage");
    free(depth_s);
    mem_free(pathname);
    exit(99);
  }
  
  // close file and free
  fclose(fp);
  free(depth_s);
  mem_free(pathname);
  
  return webpage;
}
