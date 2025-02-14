CC = gcc

CFLAGS = -Wall -pthread

TARGET = threads

SRC = threads.c

all: $(TARGET)


$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)


clean:
	rm -f $(TARGET)