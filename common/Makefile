# Makefile for the common library
#   Builds and compiles all utility functions into a library
# 
# Tim (Kyoung Tae) Kim, CS50 Lab4
# May, 2021

L = ../libcs50

# object files, and the target library
OBJS = pagedir.o index.o word.o
LIB = common.a
LLIBS = $L/libcs50.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
CC = gcc
MAKE = make

# Build $(LIB) by archiving object files
$(LIB): $(OBJS) $(LLIBS)
	ar cr $(LIB) $(OBJS)

# Dependencies
pagedir.o: pagedir.h
mem.o: mem.h
webpage.o: webpage.h
index.o: index.h
word.o: word.h

.PHONY: clean


# clean up after compilation
clean:
	 rm -f core
	 rm -f $(LIB) *~ *.o
