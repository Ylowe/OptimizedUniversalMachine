#
# Makefile for the UM Test lab
# 
CC = gcc
IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -O2 -flto -Wall -Wextra -pedantic $(IFLAGS)
LDFLAGS = -g 
LDLIBS  = -lm 

EXECS   = um

all: $(EXECS)

um:	um.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

