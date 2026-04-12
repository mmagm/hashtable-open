CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = wordcount
SRCS = wc.c hashtable.c hash.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

stress: stress.o hashtable.o hash.o
	$(CC) $(CFLAGS) -o stress stress.o hashtable.o hash.o

valgrind: stress
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./stress

clean:
	rm -f $(OBJS) $(TARGET)
