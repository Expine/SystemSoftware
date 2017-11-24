# swtest
# Simple examples for understanding the usage of xv6 swtch
# Copyright (C) 2017 Takuo Watanabe

LIB_SRCS = thread.c
ASM_SRCS = swtch.S
EXE_SRCS = thtest1.c thtest2.c thtest3.c
LIB_OBJS = $(LIB_SRCS:%.c=%.o) $(ASM_SRCS:%.S=%.o)
ALL_OBJS = $(EXE_SRCS:%.c=%.o) $(LIB_OBJS)
EXES = thtest1 thtest2 thtest3

CC = gcc
CPPFLAGS =
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -g -m32 -mstackrealign
LDFLAGS =
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY: all clean allclean

all: $(EXES)

thtest1: $(LIB_OBJS) thtest1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

thtest2: $(LIB_OBJS) thtest2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

thtest3: $(LIB_OBJS) thtest3.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(EXES)
	$(RM) $(ALL_OBJS)

allclean: clean
	$(RM) *.o a.out
	$(RM) -r *.dSYM
	$(RM) *~