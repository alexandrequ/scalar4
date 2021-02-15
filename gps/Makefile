CC=g++
SRC=$(wildcard ./src/*.c)
EXEC=main
OBJ=$(SRC:.c=.o)
INCLUDE=./include

%.o:./src/%.cpp
	$(CC) -c -I $(INCLUDE) -o $@ $^

main:main.o GPS.o
	$(CC) -I $(INCLUDE) -o $@ $^

run:main
	./main
