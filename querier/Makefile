# Makefile for the 'querier' program that utilizes following modules:
# bag, counters, index, pagedir, mem and file
#
# Tim (Kyoung Tae) Kim, CS50 Lab6
# May, 2021

# define directories
L = ../libcs50
C = ../common

# object files and libraries
OBJS1 = querier.o
OBJS2 = fuzzquery.o
LIBS =
LLIBS = $C/common.a $L/libcs50.a   

# uncomment for memtest or debugging
#TESTING=-DMEMTEST
#TESTING=-DDEBUG

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$C -I$L
CC = gcc
MAKE = make

.PHONY: all clean

all: querier fuzzquery

querier: $(OBJS1) $(LLIBS)
	 $(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Dependencies: object files depend on header files
$(OBJS1): $L/bag.h $L/hashtable.h $L/webpage.h $L/mem.h $L/file.h
$(OBJS1): $C/pagedir.h $C/index.h $C/word.h

fuzzquery: $(OBJS2) $(LLIBS)
	   $(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Dependencies: object files depend on header files
$(OBJS2):  $L/mem.h $L/file.h

test:
	bash -v ./testing.sh 


# example valgrind using fuzzquery
valgrind:
	./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 100 0 | valgrind -v --leak-check=full --show-leak-kinds=all ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index


# clean up after compilation
clean:
	 rm -f core 
	 rm -f querier
	 rm -f fuzzquery
	 rm -f *~ *.o
	 rm -rf *.dSYM

