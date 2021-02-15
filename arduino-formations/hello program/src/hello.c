#include <avr/io.h>//include this library to add the name of the registers
#include <util/delay.h>//include this one to use the function _delay_ms
#include <stdio.h> 

int main(void){
	while(1){
		printf("je suis beau\n");
		_delay_ms(1000);
	}

	return 0;
}
