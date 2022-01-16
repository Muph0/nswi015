.PHONY: all test clean
include config.mk

F = @printf "\e[1;34mmake $@:\e[0m "

CC = gcc
CFLAGS += -g -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -Wmissing-prototypes $(EXTRA_CFLAGS)

MAKEFLAGS += --no-builtin-rules --no-builtin-variables
DEPFLAGS = $(CFLAGS) -MT $*.o -MF $*.dep -MP

ALL_SRC = \
	src/alist.c \
	src/rbtree.c \
	$(TEST_SRC)

LIB_BIN = libads.a
ALL_OBJ = $(addsuffix .o, $(basename $(ALL_SRC)))
ALL_DEP = $(addsuffix .dep, $(basename $(ALL_SRC)))

-include build/$(TEST_NAME)/Makefile

all: $(LIB_BIN)
test:
	$F
	./tools/tester.sh suite tests/all.suite

clean:
	$F
	rm -rf build
	$F
	find . -iname '*.o' -delete
	$F
	find . -iname '*.dep' -delete

$(LIB_BIN): $(ALL_OBJ)
	$F
	ar -rcvs $@ $^

$(TEST_BIN): $(ALL_OBJ) tests/common.o
	$F
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$F
	$(CC) -MMD $(CFLAGS) -o $@ -c $<
#	@cat $*.dep

# Deps

%.dep: %.c
	$F
	$(CC) -MM $(DEPFLAGS) $^
#	@cat $*.dep

-include $(ALL_DEP)

