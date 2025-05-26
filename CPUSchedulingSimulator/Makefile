CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = main

SRCS = main.c process.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
