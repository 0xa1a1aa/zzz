CC = gcc
CFLAGS = -I../include/ -Wall -Wextra -Werror

SRC_DIR = ../src
SRCS = $(wildcard *.c)
TESTS = $(SRCS:%.c=%)

.PHONY: all clean

all: $(TESTS)
	@exec ./$(TESTS)

test_llist: test_llist.c $(SRC_DIR)/llist.c
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -vf $(TESTS)
