#!/bin/bash
#
# Simple testing script for crawler.c
#
# Tim (Kyoung Tae) Kim, CS50 Lab4
# April, 2021

# make clean first if files already exist
mkdir ../data
mkdir ../data/letters-{0..2}
mkdir ../data/letters-10
mkdir ../data/toscrape-{0..3}
mkdir ../data/wikipedia-{0..2}

# copy empty file to compare 
cp -r ../data ../data-1

# erroneous arguments: missing arguments
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-2 
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html

# erroneous arguments: invalid directory or depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/noname 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-2 -1

# valgrind with toscrape
valgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape-1 1

# run with all three CS website
# letters at depth 0, 1, 2 and 10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-0 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-1 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-2 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-10 10

# toscrape at depth 0, 1 
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape-0 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape-1 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape-2 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape-3 3

# wikipedia at depth 0, 1 and 2 
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia-0 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia-1 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia-2 2

diff -r ../data ../data-1 || exit 0
