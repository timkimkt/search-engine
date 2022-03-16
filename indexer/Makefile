# Makefile for the 'indexer' program that utilizes following modules:
# bag, hashtable, webpage, pagedir, mem & file
#
# Tim (Kyoung Tae) Kim, CS50 Lab5
# May, 2021

# define directories
L = ../libcs50
C = ../common

# object files and libraries
OBJS1 = indexer.o
OBJS2 = indextest.o
LIBS =
LLIBS = $C/common.a $L/libcs50.a  

# uncomment for memtest or debugging
#TESTING=-DMEMTEST
#TESTING=-DDEBUG

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$C -I$L
CC = gcc
MAKE = make

.PHONY: all clean

all: indexer indextest


indexer: $(OBJS1) $(LLIBS)
	 $(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Dependencies: object files depend on header files
$(OBJS1): $L/bag.h $L/hashtable.h $L/webpage.h $C/pagedir.h $C/index.h $L/mem.h $L/file.h $C/word.h

indextest: $(OBJS2) $(LLIBS)
	   $(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(OBJS2): $L/bag.h $L/hashtable.h $L/webpage.h $C/pagedir.h $C/index.h $L/mem.h $L/file.h $C/word.h

test:
	bash -v ./testing.sh 

# destination for index file
D = ./indexfile

# example valgrind 
valgrind:
	valgrind ./indexer ~/cs50-dev/shared/tse/output/letters-0 $D/letters-0.index 
	valgrind ./indextest $D/letters-0.index $D/letters-0.index.test 

# clean up after compilation
clean:
	 rm -f core 
	 rm -f indexer
	 rm -f indextest
	 rm -f *~ *.o
	 rm -rf *.dSYM
	 rm -rf $D
	 rm -rf $(D)-1


