# CS50 TSE Indexer
## Implementation Spec

In this document we reference the REQUIREMENTS.md and DESIGN.md and focus on the implementation-specific decisions.

Here we focus on the following:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use three data structures: an 'index' that contains a 'hashtable' that is keyed by _word_ and contains a 'counters' item. In turn, the 'counters' is keyed by _docID_ and the number of occurences of the word in the document. All data structures start empty.

The size of the index (hashtable) cannot be determined in advance, so we use a range between 200 and 900.

## Control flow

The Indexer is implemented in one file `indexer.c`, with four functions.

### main

The `main` function simply calls `parseArgs` and `indexBuild`, then exits zero.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, call `pagedir_validate(pageDirectory)`
* for `indexFilename`, ensure it is an file that can be written
* if any trouble is found, print an error to stderr and exit non-zero.

### indexBuild

Do the real work of building an index from `pageDirectory` and save the index in `indexFilename`.
Pseudocode:

	initialize the index and load webapge from pageDirectory using pagedir_load
	while webpage from pagedir_load is not null
		pass the webpage and docID to indexPage
		delete the webpage
		increment docID
	save to index
	delete index 

### indexPage

This function implements the *indexPage* mentioned in the design.
Given a `webpage`, `docID` and `index`, it scan the given page to add words not already seen into the index. It also adds a counter that is keyed to the document ID. For words that we have already seen, a counter is added or incremented for the document.

Pseudocode:

	while there is another word in the page
		if length of the word is equal to or greater than 3,
			pass the word to normalizeWord
			if word not found in index,
				create a counters_t for it
				insert word and counter into index
                        else increment the occurence of word for this doc
                 free the normalized word
         free word

## Other modules

### pagedir

We create a re-usable module `pagedir.c` to handles `pagedir_validate` that checks if it is a directory created by crawler and `pagedir_load` that loads a webpage from the file in the directory.

### index

We create a re-usable module `index.c` that is essentially a wrapper of the hashtable module. The following functions have been added: `index_save` that write the _word_ and _docID_, _count_ pair to the index while iterating through the `hashtable` of words and `counters` for each word.

### word

We create a re-usable module `word.c` that normalizes the word by converting each character into lowercase. 

### libcs50

We leverage the modules of libcs50, most notably `hashtable`, 'counters`, `file` and `webpage`.
See the directory for module interfaces.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void indexBuild (char* pageDirectory, char* indexFilename);
static void indexPage (webpage_t* webpage, int docID, index_t** idx);
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_validate (const char* pageDirectory);
webpage_t* pagedir_load (const char* pageDirectory, const int docID);
```

### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `index.h` and is not repeated here.

```c
typedef struct index index_t;
index_t* index_new(const int slots);
void index_delete(index_t* idx);
counters_t* index_find(index_t* idx, const char* key);
bool index_insert(index_t* idx, char* word, counters_t* cntr);
void index_save(FILE* fp, index_t* idx);
index_t* indexLoad(char* indexFilename);
```
### word

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `word.h` and is not repeated here.

```c
char* normalizeWord (char* word);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_validate` returns false if there is no `.crawler` file, allowing the indexer to decide what to do.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

A program _indextest_ will serve as a unit test for the index module; it reads an index file into the internal index data structure, then writes the index out to a new index file.

### Integration/system testing

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.
Second, run _indexer_ on various pageDirectories and use _indextest_ as way to validate the resulting index.
Third, a run with valgrind on both _indexer_ and _indexttest_ to ensure no memory leaks.