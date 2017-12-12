LIBS = -lpthread
CC = gcc
CFLAGS = -Wall -Werror
OUTPUT = ServerTest.out
SOURCES = ServerTest.c

all:
	$(CC) $(SOURCES) $(LIBS) $(CFLAGS) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

fresh:
	$(CLEAN) $(all)
