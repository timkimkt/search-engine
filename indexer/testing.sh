#!/bin/bash
#
# Simple testing script for indexer.c and indextest.c
#
# Tim (Kyoung Tae) Kim, CS50 Lab5
# May, 2021

dest='./indexfile'

# make clean first if files already exist
mkdir $dest

# copy empty file to compare 
cp -r $dest "${dest}-1"

# various invalid arguments (zero, one, three)
./indexer 
./indexer ~/cs50-dev/shared/tse/output/letters-0
./indexer ~/cs50-dev/shared/tse/output/letters-0 letters-0.index letters-0.index-1

# invalid pageDirectory (non-existent, not crawler)
./indexer ~/cs50-dev/non/existent/dir/letters-0 letters-0.index
./indexer ~/cs50-dev/shared/COVID-19 letters-0.index

# invalid indexfile (non-existent, read-only directory, read-only file)
./indexer ~/cs50-dev/shared/tse/output/letters-0 ./non/existent/letters-0.index 
./indexer ~/cs50-dev/shared/tse/output/letters-0 ~/cs50-dev/shared/COVID-19
./indexer ~/cs50-dev/shared/tse/output/letters-0 ~/cs50-dev/shared/COVID-19/LICENSE

# valgrind on indexer.c
valgrind ./indexer ~/cs50-dev/shared/tse/output/letters-0 ${dest}/letters-0.index 

# valgrind on indextest.c
valgrind ./indextest ${dest}/letters-0.index ${dest}/letters-0.index.test 

# print nothing if there are no differences 
~/cs50-dev/shared/tse/indexcmp ${dest}/letters-0.index ${dest}/letters-0.index.test

# run indexer on variety of pageDirectories and test with indexcmp

./indexer ~/cs50-dev/shared/tse/output/letters-10 ${dest}/letters-10.index 
./indextest ${dest}/letters-10.index ${dest}/letters-10.index.test 
# print nothing if there are no differences 
~/cs50-dev/shared/tse/indexcmp ${dest}/letters-10.index ${dest}/letters-10.index.test

./indexer ~/cs50-dev/shared/tse/output/wikipedia-1 ${dest}/wikipedia-1.index 
./indextest ${dest}/wikipedia-1.index ${dest}/wikipedia-1.index.test 
# print nothing if there are no differences 
~/cs50-dev/shared/tse/indexcmp ${dest}/wikipedia-1.index ${dest}/wikipedia-1.index.test

./indexer ~/cs50-dev/shared/tse/output/wikipedia-2 ${dest}/wikipedia-2.index 
./indextest ${dest}/wikipedia-2.index ${dest}/wikipedia-2.index.test 
# print nothing if there are no differences 
~/cs50-dev/shared/tse/indexcmp ${dest}/wikipedia-2.index ${dest}/wikipedia-2.index.test

./indexer ~/cs50-dev/shared/tse/output/toscrape-1 ${dest}/toscrape-1.index 
./indextest ${dest}/toscrape-1.index ${dest}/toscrape-1.index.test 
# print nothing if there are no differences 
~/cs50-dev/shared/tse/indexcmp ${dest}/toscrape-1.index ${dest}/toscrape-1.index.test

./indexer ~/cs50-dev/shared/tse/output/toscrape-2 ${dest}/toscrape-2.index 
./indextest ${dest}/toscrape-2.index ${dest}/toscrape-2.index.test 
# print nothing if there are no differences 
~/cs50-dev/shared/tse/indexcmp ${dest}/toscrape-2.index ${dest}/toscrape-2.index.test
