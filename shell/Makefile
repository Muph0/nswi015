.PHONY: all clean list run test-all test-1 test-2

# Settings

F = @printf "\e[1;34mmake $@:\e[0m "
MAKEFLAGS += --no-builtin-rules --no-builtin-variables

CC = gcc
CFLAGS = -g -std=c99 -posix -O2 \
	$(addprefix -I, $(HEADER_DIRS)) \
	$(addprefix -D, $(DEFINES))\
	-Wall -Wextra -Werror \
	-Wno-error=unused-parameter -Wno-error=unused-function \
	-Wno-error=unused-variable -Wno-error=unused-but-set-variable \
	-Wno-unused-function
LD = gcc
LFLAGS =
DEPFLAGS = $(CFLAGS) -MM -MT $*.o -MF $*.dep -MP

DEFINES = YYDEBUG _POSIX_C_SOURCE=200112 _XOPEN_SOURCE

# Files

SHELL_BIN = mysh
ALL_BIN = $(SHELL_BIN)

ALL_SRC = $(LIB_SRC) $(GEN_SRC) $(ENTRY_SRC)
LIB_SRC = $(filter-out $(ENTRY_SRC) $(GEN_SRC), $(wildcard src/*.c))
GEN_SRC = $(BISON_SRC) $(FLEX_SRC)
ENTRY_SRC = $(addprefix ./src/, $(addsuffix .c, $(basename $(ALL_BIN))))

ALL_DEP = $(addsuffix .dep, $(basename $(ALL_SRC)))
ALL_OBJ = $(addsuffix .o, $(basename $(ALL_SRC)))

HEADER_DIRS = inc ../ads/inc
STATIC_LIBS = ../ads/libads.a

BISON_FILES = $(wildcard src/*.y)
BISON_SRC = $(addsuffix .yy.c, $(basename $(BISON_FILES)))
FLEX_FILES = $(wildcard src/*.l)
FLEX_SRC = $(addsuffix .yy.c, $(basename $(FLEX_FILES)))

# Rules

all: $(ALL_BIN)

run: all
	./mysh

clean:
	$F
	rm -f src/*.o src/*.dep src/*.yy.c inc/*.yy.h inc/*.gch
	$F
	rm -f *.o *.dep
	$F
	rm -f $(SHELL_BIN)

mysh: $(ALL_OBJ) $(STATIC_LIBS)
	$F
	$(LD) $(LFLAGS) $^ -o $@
#	@sed -i 's/"defines" *: *\[.*\]/"defines": \[$(foreach def,$(DEFINES),"$(def)",)\]/' \
#	'.vscode/c_cpp_properties.json'

%.o: %.c %.dep
	$F
	$(CC) $(CFLAGS) -c $(realpath $<) -o $@

$(BISON_SRC): %.yy.c: %.y
	$F
	bison --header=inc/parser.yy.h -o $@ $(realpath $<)

$(FLEX_SRC): %.yy.c: %.l
	$F
	flex -o $@ $(realpath $<)

../ads/libads.a:
	$F
	make -C ../ads

test-all: trace-disable mysh
	$F
	cd tests; ./run-tests.sh
test-1: trace-disable mysh
	$F
	cd tests; ./run-tests.sh `cat ./phase-1.tests`
test-2: trace-disable mysh
	$F
	cd tests; ./run-tests.sh `cat ./wip.tests`

trace-enable:
	$F
	sed 's/TRACE 0/TRACE 1/' -i inc/debug.h
trace-disable:
	$F
	sed 's/TRACE 1/TRACE 0/' -i inc/debug.h


list: # check all paths are correctly set up
	$F
	@echo ""
	@echo ALL_BIN = $(ALL_BIN)
	@echo HEADER_DIRS = $(HEADER_DIRS)
	@echo LIB_SRC = $(LIB_SRC)
	@echo ENTRY_SRC = $(ENTRY_SRC)
	@echo GEN_SRC = $(GEN_SRC)
	@echo ALL_DEP = $(ALL_DEP)
	@echo ALL_OBJ = $(ALL_OBJ)
	@echo BISON_SRC = $(BISON_SRC)
	@echo FLEX_SRC = $(FLEX_SRC)
	@echo ""
	@ls $(HEADER_DIRS)

# Deps

.PRECIOUS: %.dep

%.dep: %.c | $(BISON_SRC) $(FLEX_SRC)
	$F
	$(CC) $(DEPFLAGS) $(realpath $<)
#	cat $*.dep

include $(wildcard $(ALL_DEP))
