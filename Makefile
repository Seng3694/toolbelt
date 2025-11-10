CC:=gcc
CFLAGS:=-g -O0 -Wall -std=c99 -fsanitize=undefined -fsanitize=address -MMD -MP

TEST_SOURCES:=$(wildcard test/*.c)
TEST_BINS:=$(patsubst test/%.c, build/%, $(TEST_SOURCES))
DEP_FILES:=$(patsubst test/%.c, build/%.d, $(TEST_SOURCES))

.PHONY: all test clean

all: $(TEST_BINS)

test: $(TEST_BINS)
	for bin in $(TEST_BINS); do ./$$bin; done

build/%: test/%.c | build
	$(CC) $(CFLAGS) $< -o $@

build:
	mkdir -p $@

clean:
	rm -rf build

-include $(DEP_FILES)

