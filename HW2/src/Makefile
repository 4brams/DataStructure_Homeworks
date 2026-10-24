CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lev

TARGET = main

all:
	$(CC) $(CFLAGS) main.c -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)