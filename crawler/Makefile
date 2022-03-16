# Makefile for the 'crawler' program that utilizes following modules:
# bag, hashtable, webpage, pagedir, mem & file
#
# Tim (Kyoung Tae) Kim, CS50 Lab4
# May, 2021

# define directories
L = ../libcs50
C = ../common

# object files and libraries
OBJS = crawler.o
LIBS =
LLIBS = $L/libcs50.a $C/common.a  

# uncomment for memtest, logging or debugging
TESTING=-DMEMTEST
#TESTING=-DAPPTEST
#TESTING=-DDEBUG

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$C -I$L
CC = gcc
MAKE = make

crawler: $(OBJS) $(LLIBS)
	 $(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Dependencies: object files depend on header files
crawler.o: $L/bag.h $L/hashtable.h $L/webpage.h $C/pagedir.h $L/mem.h $L/file.h

.PHONY: all clean

all: crawler

test:
	bash -v ./testing.sh 

# example valgrind 
#valgrind:
	valgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toscrape-1 1


# clean up after compilation
clean:
	 rm -f core 
	 rm -f crawler
	 rm -f *~ *.o
	 rm -rf *.dSYM
	 rm -rf ../data
	 rm -rf ../data-1
