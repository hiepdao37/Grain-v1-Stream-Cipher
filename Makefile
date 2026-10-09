CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SRC = src/main.c src/grain.c
TEST_SRC = tests/test_grain.c src/grain.c

.PHONY: all test clean

all: grain

grain: $(SRC) src/grain.h
	$(CC) $(CFLAGS) $(SRC) -o grain

test: test_grain
	./test_grain

test_grain: $(TEST_SRC) src/grain.h
	$(CC) $(CFLAGS) $(TEST_SRC) -o test_grain

clean:
	rm -f grain test_grain