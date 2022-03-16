# CS50 TSE Querier
## Implementation Spec

In this document we reference the REQUIREMENTS.md and DESIGN.md and focus on the implementation-specific decisions.

Here we focus on the following:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We define a wordlist struct that contains an array of strings and integer count of number of strings. We use counters from libcs50 to merge or intersect queries with the iterator. We create a intersectArgs struct to pass in two counters as args to the iterator. Finally, we create a rankitem struct that contains the ID and final count from the query. We use this struct to sort and rank the query by descending order. 

## Control flow

The Querier is implemented in one file `querier.c`, with several functions. Most important functions are detailed below.

### main

The `main` function calls `parseArgs`, `indexload`, `tokenize`, `validateQuery`, `cleanQuery, `query`, `andSequence`,`rankResult` and `index_delete`, then exits zero.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, ensure it is created by crawler and at least one file exists
* for `indexFilename`, ensure it is an file that can be read
* if any trouble is found, print an error to stderr and exit non-zero.

### tokenize

Parse a line of chars into an array of strings. 

Pseudocode:

	initialize an empty array of strings
            for each char in the line read
                while char is empty space, move along
                check if char is a non letter char
                if the char is a letter char
		mark as beginning of word and move along
                check for nonletter char in middle of word
                terminate string will null char
	        normalize word and insert into array
	    insert array and size into wordlist struct 

### validateQuery

Validate query as specified in the requirement spec.

Pseudocode:

	for each word returned by tokenize
            return false if or is the first word
            return false if and is the first word
            return false if or is the last word
            return false is and is the last word
            return false for two consecutive 'and'
            return false for two consecutive 'or'
            return false for adjacent 'and' 'or'
	else return true

### cleanQuery

Simply prints out a clean query of words after it is normalized in tokenize and validated by validateQuery. 

Pseudocode:

	initialize an empty array of strings
            for each char in the line read
                while char is empty space, move along
                check if char is a non letter char
                if the char is a letter char
		mark as beginning of word and move along
                check for nonletter char in middle of word
                terminate string will null char
	        normalize word and insert into array
	    insert array and size into wordlist struct 

### query

Query is where the counters for each word is iterated over with different helper functions depending on whether it is an _and_ or _or_ sequence. We first process intersect all the _and_ queries and unionize the _or_ queries at the end.

Pseudocode:

	for words that have been tokenized
            if the word is an 'or'
                pass temp array and word count to andSequence
                free temp array
                set word count to zero 
            else
                if the word is an 'and'
                    continue;
                add words to temp array
                increment array word count
        pass temp array and word count to andSequence
        insert counter returned to bag
        while bag is not empty
            iterate over result counter and get union 
            delete counters
        delete wordlist
        delete bag
        return result counter


### andSequence

andSequence copies counters from index_find and iterates over counters for remaining words to find the intersection of the two counters. A helper function is used to set the count to the lower value if the key exists in both counters and set the key to zero if it doesn't exist in either one of the counter.

Pseudocode:

	for words in temp array 
            copy counter from index_find to temp counter
            if there is only one word
                return temp counter
            if there are more words
                if index_find returns a counter
                iterate counter and set count to lower value
        return temp counter

### rankResult

rankResults first counts the number of items in the counter that holds the final result. It prints "No documents match" if the count is zero. It iterates once more to put each item into a rankitem struct that contains the integer key and count for each counters node. It then sorts the rankitems based on the count (score) and prints out the result in descending order. 

Pseudocode:

	iterate and count items in result counter
        print message and return if count is zero
        create an array of rankitem structs
        iterate over counters and insert into rankitem struct
        intersert struct into array
        sort array by score (count) in descending order
        for each rankitem in array
            load URL from pageDirectory
            print out score, ID and URL 

## Other modules

### indexer

We use the indexer module to load the index, look if words exist in the index and access the _docID, count_ pairs for words queried. 

### pagedir

In parseArgs, we use`pagedir_validate` that checks if it is a directory created by crawler and `pagedir_load` to check that at least one file in the directory.

### word

We use  module `word.c` that normalizes the word by converting each character into lowercase. 

### libcs50

We leverage the modules of libcs50, most notably `file` to read stdin and `counters` to store docID and count returned from index. We make good use of the counters_iterate function by passing in structs as args and using helper functions to obtain the intersection or union of two sets. We also use the bag module to store counters and other structs.

See the directory for module interfaces.

## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here.

```c
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

static void counters_copy_helper(void* arg, const int key, const int count);
static void counters_and_helper(void* arg, const int key, const int count);
static void counters_or_helper(void* arg, const int key, const int count);

static void itemcount(void* arg, const int key, const int count);
static void itemdelete(void* item);

static void insertStruct (void* arg, const int key, const int count);
static int qsort_helper (const void* itemA, const void* itemB);
```


## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_validate` returns false if there is no `.crawler` file, allowing the querier to decide what to do.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

A program _fuzzquery_ is a program that can be used as a unit test for the querier module; it randomly generates queries that can be passed to `querier.c` The user can choose to provide an optional seed that allows testing of the same input.

### Integration/system testing

We write a script `testing.sh` that invokes the querier several times, with a variety of command-line arguments.

First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.

Second, run _querier_ on various queries that are randomly generated through  `fuzzquery.c` as a way to validate that various valid and invalid queries can be handled.

Third, a run with valgrind on _querier_ to ensure no memory leaks.