CC = gcc
CFLAGS = -std=c17 -Wall -Wextra
TARGET = traceroute
SRC = main.c
TAR_NAME = antoni_strasz

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

