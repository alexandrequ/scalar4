#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

int main(void){
	cli();
	TCCR1B=(TCCR1B & ~(111<<CS10)) | (101<<CS10);//clock/1024
	TIMSK1|=(1<<TOIE1);
	DDRB|=(1<<5);//pin D13 en sortie
	int count=0;
	sei();
	
	
	
	while(1){
		
	}
	return 0;
	
}

ISR(TIMER1_OVF_vect){
	PORTB = (PORTB & ~(1<<5)) | ~(PORTB & (1<<5));
	
}
