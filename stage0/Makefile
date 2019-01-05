#
# Makefile for the UM Test lab
# 
CC = gcc
IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64
LDLIBS  = -l40locality -lm -lbitpack -lcii40 -lrt

EXECS   = memory, um

all: $(EXECS)

memory: memory.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um:	memory.o memory_interface.o alu.o io.o um.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

