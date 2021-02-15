#include "macros.h"
#include <avr/io.h>
#include <stdio.h>


int main(int argc, char** argv){
	DDRD=set_bit(DDRD,DDD6);
	TCCR0A=write_field(TCCR0A,WGM00,2,0b11);
	TCCR0A=write_field(TCCR0A,COM0A0,2,0b10);
	TCCR0B=write_field(TCCR0B,CS00,3,0b001);
	OCR0A=100;
	
	while(1){
		printf("hello\n");
	}
	return 0;
}
