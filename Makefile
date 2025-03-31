CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra
SRC = handlers.c helpers.c traceroute.c
OBJ = handlers.o helpers.o traceroute.o
EXEC = traceroute

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

handlers.o: handlers.c handlers.h
	$(CC) $(CFLAGS) -c handlers.c -o handlers.o

helpers.o: helpers.c handlers.h
	$(CC) $(CFLAGS) -c helpers.c -o helpers.o

traceroute.o: traceroute.c 
	$(CC) $(CFLAGS) -c traceroute.c -o traceroute.o


clean:
	rm -f $(OBJ)

distclean: clean
	rm -f $(OBJ) $(EXEC)


test:
	sudo ./traceroute 8.8.8.8