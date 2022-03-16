# CS50 Lab 5
## CS50 Spring 2021

### indexer

The indexer is a standalone program that reads document files created by TSE crawler to build and write an index. indextest.c can be used to read this index and save it to another file.
 
### Usage

The indexer's only interface is the command-line where the user must always provide two arguments

```bash
$ indexer pageDirectory indexFilename
```

### Implementation

The `indexer` closely follows the Indexer implmentation spec. We use three data strucutres: an 'index' module that creates a 'hashtable' of nontrivial words found from webpage in pageDirectory and 'counters' of docID and number of occurences in the document. The size of the index(hashtable) is intiailiazed at 200.

`indexer.c` has four functions.

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void indexBuild (char* pageDirectory, char* indexFilename);
static void indexPage (webpage_t* webpage, int docID, index_t** idx);
```

#### main

The `main` function calls `parseArgs` and `indexBuild`, then exits zero. 

#### parseArgs

The `parseArgs` function extracts function parameters from the command line. It returns only if sucessful.

* for `pageDirectory`, call `pagedir_validate(pageDirectory)`
* for `indexFilename`, ensure it is an file that can be written
* if any trouble is found, print an error to stderr and exit non-zero.

### Assumptions

`indexer` assumes that the `pageDirectory` has filed named with nonzero positive integers without any gaps. `indexer` and `indextest` assumes that the contents of files in the directory follow the format detailed in the specs. `testing.sh` will creates folders to store files written by index but make clean is required if the folders already exist.

### Files

* `IMPLEMENTATION.md` - implmentation spec
* `Makefile` - compilation procedure
* `indexer.c` - implementation
* `indextest.c` - unit test 
* `testing.sh` - test script
* `testing.out` - result of `make test &> testing.out`

### Compilation

To compile indexer and indextest, simply `make all`.

### Testing

To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

Try testing with `MEMTEST` or `DEBUG` by editing Makefile to turn on the flag and then `make test`.

To test with valgrind, `make valgrind`. This will run valgrind on indexer and indextest on sample inputs. The same test is carried out in `testing.sh`.