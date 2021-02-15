/*
 * File:   communication_I2C.c
 * Author: alexis
 *
 * Created on 18 mars 2019, 21:44
 */

#include <stdio.h>
#include <stdlib.h>
#include "communication_I2C.h"



void init_I2C1(){
    I2C1CONbits.I2CEN = 1; //enable le bus
    I2C1BRG=19;
    I2C1CONbits.SCLREL=1;
}


void write_I2C1(uint8_t slave_add,uint8_t *data, int size){ //type des arguments : un octect et un array d'octect

    int i =0;
    uint8_t add = (slave_add << 1) | 0b0;   //set l'adresse pour y écrire


    I2C1CONbits.SEN = 1; //start sequence
    while(I2C1CONbits.SEN);

    I2C1TRN = add; // écrit le registre directement

    while(I2C1STATbits.TRSTAT);//attendre que la transmission se termine

    for(i=0;i<size;i++){
        I2C1TRN = data[i]; //les données à envoyer au slave, contenant l'adresse du registre
        while(I2C1STATbits.TRSTAT);

    }

    I2C1CONbits.PEN = 1; //stop
    while(I2C1CONbits.PEN);
    //_LATB15=0;

}


void read_I2C1(uint8_t slave_add, uint8_t *data, int size){ //sreg_add : adresse du regsitre du slave. Renvoi l'array de donnée lue

    int i =0;
    // rq : rajouter uint__t sreg_add si on a l'adresse du registre
    uint8_t add = (slave_add << 1) | 0b1;   //set l'adresse pour y lire


    I2C1CONbits.SEN = 1; //start sequence
    while(I2C1CONbits.SEN==1);

    I2C1TRN = add; // écrit le registre directement

    while(I2C1STATbits.TRSTAT);


    for(i=0;i<size;i++){
        I2C1CONbits.RCEN=1;//mode reception activé
        while(I2C1CONbits.RCEN==1);
        data[i] = I2C1RCV;//lire
        _ACKEN = 1;

        while(I2C1CONbits.ACKEN);

    }
    I2C1CONbits.PEN = 1; //stop
    while(I2C1CONbits.PEN==1);




}
