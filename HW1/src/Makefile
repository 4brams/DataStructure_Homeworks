CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lev

TARGET = calculator

all:
	$(CC) $(CFLAGS) calculator.c -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)