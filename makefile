CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TEST = allocator_test
OBJS = main.o test.o

.PHONY: all clean test

all: $(TEST)

$(TEST): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

allocator.o: main.c allocator.h
	$(CC) $(CFLAGS) -c $<

allocator_test.o: test.c allocator.h
	$(CC) $(CFLAGS) -c $<

test: $(TEST)
	./$(TEST)

clean:
	rm -f *.o $(TEST)
