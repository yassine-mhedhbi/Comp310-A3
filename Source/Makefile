# @author     :   Shabir Abdul Samadh (shabirmean@cs.mcgill.ca)
# @purpose    :   COMP310/ECSE427 Operating Systems (Assingment 3) - Phase 2
# @file		  :	  Makefile for compilation
# @compilation:   Use "make container"

CC=gcc
LIBS=-lseccomp -lcap
CFLAGS=-g -Wall -Werror
SOURCE1=sr_container.c sr_container_helpers.c sr_container_utils.c
EXEC1=SNR_CONTAINER

.PHONY: clean dist container

container: $(SOURCE1)
	$(CC) -o $(EXEC1) $(CFLAGS) $(SOURCE1) $(LIBS)

clean:
	rm $(EXEC1) COMP310-A3-Mhedhbi-Wolman.tar.gz

dist: COMP310-A3-Mhedhbi-Wolman.tar.gz

COMP310-A3-Mhedhbi-Wolman.tar.gz: sr_container.c sr_container_helpers.c REPORT.md
	tar -czf $@ $^
