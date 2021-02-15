#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRB|=(1<<5);//pin D13 en sortie
	while(1){
		PORTB |= (1<<5);
		_delay_ms(500);
		PORTB &= ~(1<<5);
		_delay_ms(500);
	}
	return 0;
	
}
