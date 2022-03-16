# CS50 Lab 5
## CS50 Spring 2021

### pagedir

The `pagedir` is a re-usable module that writes a page to `pageDirectory`, marks it as a crawler-produced `pageDirectory`,  This module is also used by the Indexer and Querier. 

### index
An ‘index’ is a hashtable of counters that contain (docID count) pairs. It acts as a wrapper for the hashtable module with local functions that help write the index file to the given directory. 

### word
A ‘word’ is a simple, re-usable module converts a given word into lower case

### Usage

`pagedir` is used by Crawler, Indexer and Querier. `word` and `index` modules are used by the Indexer and Querier.  

### Implementation

The `pagedir` closely follows the CS50 TSE Crawler implmentation spec. `index` and `word` modules follow the Indexer implementation spec. 

`pagedir.c` has four functions.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID
bool pagedir_validate (const char* pageDirectory); 
webpage_t* pagedir_load (const char* pageDirectory, const int docID);
```

`index.c` has seven functions.

```c
typedef struct index index_t;
index_t* index_new(const int slots);
void index_delete(index_t* idx);
counters_t* index_find(index_t* idx, const char* key);
bool index_insert(index_t* idx, char* word, counters_t* cntr);
void index_save(index_t* idx, char* indexFilename);
index_t* indexLoad(char* indexFilename);
```

`word.c` has one function.
```c
char* normalizeWord (char* word);
```

### Files

* `Makefile` - compilation procedure
* `pagedir.h` - the interface
* `pagedir.c` - the implementation
* `word.h` - the interface
* `word.c` - the implementation
* `index.h` - the interface
* `index.c` - the implementation

### Compilation

To compile, simply type `make`.

### Testing

`pagedir`, `indexer`, and `word` modules do not have their own test script but they can be tested by `testing.sh` in the `indexer` module. See README.md in `indexer` for more details. 
