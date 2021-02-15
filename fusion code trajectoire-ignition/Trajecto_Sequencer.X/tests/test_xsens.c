#include "xbus.h"
#include <timing.h>
#include <clock.h>
#include <xc.h>

void periodic_action() {
    _LATA0 = ~_LATA0;
}

void periodic_action2() {
}

int main() {
    
    // Configuration de la PLL pour passer à 128 MHz
    _PLLPRE = 6;
    _PLLPOST = 0;
    _PLLDIV = 126;
    
    // Configuration du mode de secours (panne du quartz) : FOSC=128.05375 MHz
    config_failsafe_clock(4, 2, 139);
    
    // ****** Association du module CAN aux pins 39 et 40 ******
    __builtin_write_OSCCONL(OSCCONL & 0xbf); // Déverrouillage du PPS
    // Réception CAN sur RP37 (pin n° 14)
    RPINR26bits.C1RXR = 37;
    // Transmission CAN sur RP38 (pin n° 15)
    RPOR2bits.RP38R = 0x000E;
    __builtin_write_OSCCONL(OSCCONL | 0x40); // Verrouillage du PPS

    // ****** Réglage des ports ********************************
    TRISA = 0x00; // port A integralement en sortie
    LATA = 0x00; // port A initialise a 0
    TRISB = 0;
    _TRISB5 = 1;
    
    init_counting();
    start_counting(1000);
    
    while(1);
    
    return 0;
}
