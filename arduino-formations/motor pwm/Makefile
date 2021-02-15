CC=avr-gcc
CFLAGS1=-W -Wall -mmcu=atmega328p -Os -DF_CPU=16000000UL -I ./include
CFLAGS2=-W -Wall -mmcu=atmega328p
BAUDRATE=115200
SRC=$(wildcard ./src/*.c)
EXEC=main
OBJ=$(SRC:.c=.o)
INCLUDE=./include

%.o:%.c
	$(CC) $(CFLAGS1) -c -o $@ $^

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS2) -o $@ $^

%.hex: %
	avr-objcopy -O ihex -R .eeprom $^ $@

flash:$(EXEC).hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:$^

monitor:
	screen -L /dev/ttyUSB0 $(BAUDRATE)

clean:
	rm ./src/*.o
	

	
