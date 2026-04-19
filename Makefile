CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = wordcount
SRCS = examples/wc.c hashtable.c hash.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -I. -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

hashtable.o: hashtable.c hashtable.h
hash.o: hash.c hash.h

tests: test/tests.o hashtable.o hash.o
	$(CC) $(CFLAGS) -I. -o test/tests.exe test/tests.o hashtable.o hash.o

test/tests.o: test/tests.c hashtable.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

valgrind: tests
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test/tests

clean:
	rm -f *.o $(TARGET) test/*.o test/*.exe examples/*.exe
