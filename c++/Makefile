CC=g++
SRC=$(wildcard ./src/*.c)
EXEC=main
OBJ=$(SRC:.c=.o)
INCLUDE=./include

%.o:%.c
	$(CC) -c -o $@ $^

hello:hello.o Person.o
	$(CC) -o $@ $^

run:hello
	./hello
