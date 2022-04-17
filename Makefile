#
# makefile for smsh
#
CC=gcc
CFLAGS=-g

mysh: mysh.o splitline.o execute.o
	$(CC) $(CFLAGS) -pthread -o mysh mysh.o splitline.o execute.o

clean:
	rm -f *.o
