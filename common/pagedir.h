/* 
 * pagedir.h - header file for CS50 'pagedir' module
 * 
 * ‘pagedir’ is a re-usable module that writes a page to the pageDirectory 
 * and marks it as a Crawler-produced directory (by creating .crawler). It 
 * validates files that have been produced by Crawler and loads webpages
 * from pageDirectory
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab4, 5
 * May, 2021 
 */

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "webpage.h"
#include "file.h"
#include "mem.h"

/********************** functions **********************/

/**************** pagedir_init ****************/
/* constructs path and saves .crawler in the directory
 * We assume:
 *   caller provides an existing directory
 * We return:
 *   true upon successfully creating the file
 *   false if error in opening directory
 * We guarantee:
 *   creation of .crawler file if valid path is given
 * Caller is responsible for:
 *   creating pageDirectory
  */
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* constructs path and writes URL, depth, contents 
 * We assume:
 *   caller provides
 * We return:
 *   nothing; print error message if can't open file
 * We guarantee:
 *   creation of pageDirectory files with unique ID
 * Caller is responsible for:
 *   providing valid page, pageDirectory and ID >= 1
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_validate() ****************/
/* validates if it is pageDirectory created by crawler
 * We assume:
 *   caller provides readable directory
 * We return:
 *   true if directory created by crawler
 *   false otherwise
 * We guarantee:
 *   
 * Caller is responsible for:
 *   providing valid pageDirectory
 */
bool pagedir_validate (const char* pageDirectory); 

/****************** pagedir_load()  ******************/
/* load webpage in pageDirectory
 * We assume:
 *   pageDirectory has files formatted as detailed in specs
 * We return:
 *   nothing; print error message if can't open file
 * We guarantee:
 *   creation of webpage from pageDirectory files
 * Caller is responsible for:
 *   providing valid pageDirectory and docID >= 1
 */
webpage_t* pagedir_load (const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H
