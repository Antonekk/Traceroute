CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra
TARGET = traceroute
SRC = traceroute.c helpers.c handlers.c
TAR_NAME = antoni_strasz

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)



test:
	sudo ./traceroute 156.17.254.113