# CS50 Lab 6
## CS50 Spring 2021

### querier

The querier is a standalone program that reads index files created by TSE indexer to process a query and return matching documents in ranked order. It handles queries that are either connected by 'and' or 'or'. Two words without any conjunction is considered an implicit 'and'.

### Functionality

All functionality mentioned in CS50 TSE Querier README.md has been implemented. The querier supports 'and' and 'or' operators with precedence. It also prints the document set in decreasing order by score, thus meeting the full specs. 

### Usage

The querier's only interface is the command-line where the user must always provide two arguments

```bash
$ querier pageDirectory indexFilename
```

### Implementation

The `querier` closely follows the Querier implmentation spec and implements several data structures.

`querier.c` has the following functions.

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
#### Implementation
Refer to the Implementation Spec for more details.

#### Design
Refer to the Design Spec for more details. 

### Assumptions

`querier` assumes that the `pageDirectory` has files named with nonzero positive integers without any gaps and  assumes that the contents of the `pageDirectory` and `indexFilename` files follow the format detailed in the Indexer specs. We also assume that the index file corresponds to the given `pageDirectory`. There are no any further assumptions that we make that differs from the Requirement Spec. 

### Files

* `IMPLEMENTATION.md` - implmentation spec
* `DESIGN.md` - design spec
* `Makefile` - compilation procedure
* `querier.c` - implementation
* `fuzzquery.c` - unit test provided by CS50 TSE 
* `testing.sh` - test script
* `testing.out` - result of `make test &> testing.out`

### Compilation

To compile querier and fuzzquery, simply `make all`.

### Testing

To test, simply `make test`. We use fuzzquery.c in `testing.sh` to test multiple, randomly generated queries. The examples folder contains a sample text file containing a query with syntax errors. 

See `testing.out` for details of testing and an example test run.

Try testing with `MEMTEST` or `DEBUG` by editing Makefile to turn on the flag and then `make test`.

To test with valgrind, `make valgrind`. This will run valgrind on querier  on sample inputs. The same test is carried out in `testing.sh`.