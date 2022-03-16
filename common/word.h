/* 
 * word.h - header file for CS50 'word' module
 * 
 * ‘word’ is a simple, re-usable module converts a given word into lower case
 * 
 * Tim (Kyoung Tae) Kim, CS50 Lab5
 * May, 2021 
 */

#ifndef __WORD_H
#define __WORD_H

#include <ctype.h>
#include "mem.h"
#include "string.h"


/* ******************* normalizeWord ************************** */
/* converts the word into lowercase
 * We assume:
 *   caller provides a valid char pointer
 * We return:
 *   a word that is normalized
 * We guarantee:
 *   
 * Caller is responsible for:
 *   freeing the word returned
 * Notes:
 *   
 */
char* normalizeWord (char* word);

#endif // __WORD_H
