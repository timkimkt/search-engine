# CS50 TSE Querier
## Design Spec

According to the Qurier Requirements Spec, the TSE *Querier* is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin. 

Here we focus on the following:

- User interface
- Inputs and outputs
- Functional decomposition into modules
- Pseudo code for logic/algorithmic flow
- Major data structures
- Testing plan

## User interface

As described in the Requirements Spec, the querier's only interface with the user is on the command-line; it must always have two arguments.

```bash
$ querier pageDirectory indexFilename
```

For example, to make a query with a pageDirectory created by TSE Crawler and indexfile created by TSE Indexer, use this command line:

``` bash
$ ./querier ~/cs50-dev/shared/tse/output/wikipedia-1 ~/cs50-dev/shared/tse/output/wikipedia-1.index                                                     
```

## Inputs and outputs

*Input:* the querier uses the indexer module to read files from directory and the index file created for the directory. Then, the querier awaits for user input from stdin. Queries are connected by 'ands' (implict or explicit) and 'ors'.

*Output:* the querier echoes the query and outputs the result of the query in the form of _score_, _docID_ and _URL_. The result of valid queries are printed in descending order in terms of score.

## Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *tokenize* parses a line into an array of normalized words
 3. *validateQuery* checks the query for invalid queries 
 4. *cleanQuery* prints out a clean query 
 5. *query* calls andSequence and process or queries 
 6. *andSequence* process implicit and explicit and queries
 7. *rankresult* ranks result based on count (score) 

And the following data structures and local structs:

  1. *wordlist* struct that contains an array of char pointers and number of words (strings)
  2. *intersectArgs* struct that contains two counters to intersect and store the result to 
  3. *rankitem* struct that contains the int ID and count of counters item
  4. *index*,a module providing the data structure to represent the in-memory index, and functions to read and write index files;
  5. *counters* a module that stores an integer key and integer count 
  6. *bag* a module that acts as a linked list and allows extraction of its item
  7. *word*, a module providing a function to normalize a word.

## Pseudo code for logic/algorithmic flow

The querier will run as follows:

    parse the command line, validate parameters, initialize other modules
    while stdin is not EOF
        Read each line, parse and validate the query
        For each and sequence
            create temp array of words and pass to andSequence
                create temp counters and return the intersect
            store results in a bag
        For each or sequence
            retrieve counters from bag
            iterate over counter and return the union
        Iterate and count items in final counter
        Iterate and place into array of rankitem struct
        Sort and print by descending order of score

## Major data structures

The key data structure is the counters that is read with the index module. We iterate over the counters of _docId_, _count_ pairs to create the intersect and union of sets required to process the queries. We use several other data structures that assit this process. We define a wordlist struct that contains an array of strings and integer count of number of strings. We create a intersectArgs struct to pass in two counters as args to the iterator. Finally, we create a rankitem struct that contains the ID and final count, which is used to sort and rank the query by descending order. 

## Testing plan

_Unit testing_ The program fuzzquery.c from CS50 will serve as a unit test for the querier module; it randomly generates queries given the number of queries and seed, which can be piped to the querirer. 

_Integration testing_ The querier, as a complete program, will be tested by comparing the resulting query to the expected result from the indexfile.

   1. Test querier with various invalid arguments, no arguments, one argument and three or more arguments. We also test with invalid pageDirectory as well as invalid indexFile.
   
   2. Test querier with various queries, with and without fuzzquery. We run querier on a variety of pageDirectories and indexFiles in the shared CS50 directory. Valid and invalid queries are provided to check for boundary cases.

   3. Finally, run valgrind on the querier to ensure no memory leaks or errors.