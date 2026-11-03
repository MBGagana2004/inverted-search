CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
TARGET = inverted_search
SRCS = display.c create.c save.c main.c update.c search.c validation.c
OBJS = $(SRCS:.c=.o)
HDR = inverted_search.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
