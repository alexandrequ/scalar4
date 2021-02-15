#include <stdio.h>
#include <stdlib.h>
#include "herkulex_controller.h"
#include "herkulex_keywords.h"
#include <xc.h>
#include "timing.h"
#include "serial.h"

void adc_init(){
	_TRISA0=1;//A0 as an input
	ANSELAbits.ANSA0=1;//A0 as an analog input
	
	AD1CON1bits.ASAM=1;//adc sample auto start bit
	AD1CON3bits.SAMC=1;//auto sample time bit
	AD1CON3bits.ADCS=0b00111111;//Tad=64*Tcy
	AD1CHS123bits.CH123SA=0;//channel 1 positive input AN0
	AD1CHS123bits.CH123NA1=0;//channel 1 negative input vref
	AD1CON1bits.ADON=1;//turn on adc
	sleep(100);
}

uint16_t adc_read(){
	AD1CON1bits.ASAM=1;//debut du sampling, normalement c est automatique mais bon
	sleep(100);
	AD1CON1bits.SAMP=0;//debut de la conversion
	while(!AD1CON1bits.DONE);
	AD1CON1bits.DONE=0;//askip c est bien  de le remettre a 0
	return ADC1BUF0;
}

int main(int argc, char** argv) {
	 __builtin_write_OSCCONL(OSCCONL & 0xbf);//deverouillage du peripheral pin select

    RPINR18bits.U1RXR=42;    
    RPOR4bits.RP43R=0x0001;
    
    __builtin_write_OSCCONL(OSCCONL | 0x40);//hold of pps
    //configuration of the uart
    uint32_t baudrate = 9600;
    config_uart1(baudrate,UART_CLOSEST);
    
    //configuration du moteur
    _PLLPRE = 6;
    _PLLPOST = 0;
    _PLLDIV = 126;
    
    uint8_t idList[1]={0x0b};//l'adresse du servo moteur qui est ecrite sur celui ci normalement, sinon ecrire 0xfd.
    size_t size=1;
    uint16_t pos[1]={0};


    init_servos();
    set_servo_max_torque(idList,size,1023);
	//
	adc_init();
    
    while(true){
        uint16_t value=adc_read();
        value=value/2+1023/2;
        sleep(100);
        pos[0]=value;
        set_servos_pos(idList,size,pos,10,false,true,false);
       
	}
	return 0;
}
