/* 
 * File:   main.c
 * Author: alexis Vandewalle
 *
 * Created on 18 mars 2019, 21:55
 */

#include <stdio.h>
#include <stdlib.h>
#include "communication_I2C.h"
#include "serial.h"
#include "timing.h"
#define SLAVE_ADD 0x76
/*
 * 
 */
int main(int argc, char** argv) {
    _TRISB15=0;
    _LATB15=1;
    
    
    _TRISB11=0;
    // Attribution des pin aux modules UART et CAN
    __builtin_write_OSCCONL(OSCCONL & 0xbf); // Déverrouillage du PPS
    // Réception UART sur RP42 (pin n° 21)
    RPINR18bits.U1RXR = 42;
    // Transmission UART sur RP43 (pin n° 22)
    RPOR4bits.RP43R = 0x0001;
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS
    
    config_uart1(9600,0);
    uint8_t byte=100;
    receive_uart1(&byte,1,1);
    //transmit_uart1(&byte,1,1);
    
    init_I2C1();
    
    uint8_t command_measure[1]={0b01001000};
    write_I2C1(SLAVE_ADD,command_measure,1);//demande d'envoie de donnees
    sleep(100);
    uint8_t command_read[1]={0b00000000};
    write_I2C1(SLAVE_ADD,command_read,1);
    
    uint8_t data[3]={0,0,0};
    read_I2C1(SLAVE_ADD,data,3);
    write_I2C1(SLAVE_ADD,command_read,1);
    read_I2C1(SLAVE_ADD,data,3);
    transmit_uart1(data,3,1);
    
    
    
    
    while(1);
    return (EXIT_SUCCESS);
}

