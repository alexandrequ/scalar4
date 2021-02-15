/**
 * branchement:
 * Reception uart pin 21
 * transmission uart pin 22
 * 
 * clock scl sur pin 17
 * sda sur pin 18
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include "timing.h"
#include "barometre.h"
#include "i2c.h"
#include "serial.h"


int main(int argc, char** argv) {
    TRISA=0;
    TRISB=0;
    _TRISB15=0;
    _TRISB8=1;
    _TRISB9=1;
    _LATB15=1;
    //initialisation de la transmission uart
    _TRISB11=0;
    _TRISB10=1;
    // Attribution des pin aux modules UART et CAN
    __builtin_write_OSCCONL(OSCCONL & 0xbf); // Déverrouillage du PPS
    // Réception UART sur RP42 (pin n° 21)
    RPINR18bits.U1RXR = 42;
    // Transmission UART sur RP43 (pin n° 22)
    RPOR4bits.RP43R = 0b000001;
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS
    
    config_uart1(9600,0);
    uint8_t byte=100;
    receive_uart1(&byte,1,1);
    init_I2C1();//initialisation i2c

    //FIRST TEST
    uint8_t eprom[SIZE_PROM]={0};
    
    init_pressure(eprom);
    transmit_uart1(eprom,SIZE_PROM,1);
    
    //SECOND TEST-transmettre une seule donnée a la fois
    
    long resultat[2]={0};
    get_measure(eprom,resultat);
    uint8_t a=resultat[0]%256;
    resultat[0]=resultat[0]-a;
    uint8_t b=resultat[0]%(256);
    resultat[0]=resultat[0]-b*256;
    uint8_t c=resultat[0]%(256);
    resultat[0]=resultat[0]-c*256;
    uint8_t pression[4]={resultat[0],c,b,a};
    transmit_uart1(pression,4,1);
    while(1);
    return (EXIT_SUCCESS);
}


