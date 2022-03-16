# CS50 Lab 4
## CS50 Spring 2021

### crawler

The `crawler` is a program that crawls the web and retrieves webpages starting from a `seed` URL. It parses the seed webpage, extracts any embedded URLs, then retrieves each of those pages, recursively, but only up the given `maxDepth`.
 
### Usage

The crawler's only interface is the command-line where the user must always provide three arguments

```bash
$ crawler seedURL pageDirectory maxDepth
```

### Implementation

The `crawler` closely follows the CS50 TSE Crawler implmentation spect. We use two data strucutres: a 'bag' that contains pages that need to be crawled and 'hashtable' of URLs that we have seen during the crawl. The size of the hashtable is intiailiazed at 200.

`crawler.c` has four functions.

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
```


#### main

The `main` fucntion calls `parseArgs` and `crawl`, then exits zero. 

#### parseArgs

The `parseArgs` function extracts function parameters from the command line. It returns only if sucessful.

* for `seedURL`, normalize and validate it is an internal URL
* for `pageDirectory`, call `pagedir_init()`
* for `maxDepth`, ensure it is an integer and within specified range
* Otherwise, print an error to stderr and exit non-zero.

### Assumptions

`crawler` assumes that the `pageDirectory` files are created in advance. `testing.sh` will create the required folders but user is required to make clean first if the folders already exist. We assume that `pageDirectory` does not contain any files whose name is an integer. The crawler also pauses at least one second between page-fetches and ignore non-internal URls. 

### Files

* `Makefile` - compilation procedure
* `crawler.h` - the interface
* `crawler.c` - the implementation
* `testing.sh` - test data
* `testing.out` - result of `make test &> testing.out`

### Compilation

To compile, simply `make crawler.o`.

### Testing

To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

Try testing with `MEMTEST` or `APPTEST` by editing Makefile to turn on the flag and then `make test`.

To test with valgrind, `make valgrind`. This will run valgrind _toscrape_ at the depth of 1. This test is also conducted in `testing.sh`.
