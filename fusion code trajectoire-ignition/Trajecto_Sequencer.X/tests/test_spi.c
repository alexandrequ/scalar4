#include "timing.h"
#include "can.h"
#include "spi.h"
#include "clock.h"
#include <stdbool.h>
#include <xc.h>

uint8_t buf[] = { 0, 0, 0, 0, 0, 0 };

struct MtsspInfo {
    uint8_t m_version;
    uint8_t m_drdyConfig;
};

void periodic_action() {
    _LATA0 = ~_LATA0;
}

void periodic_action2() {
    transmit_can((Canframe){0x120, buf, sizeof(buf)}, true);
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
#if 0
    config_spi1(SPI_PS1_1_64, SPI_PS2_1_8, 1, 4, 3); // bitrate de 125 kbps
    _LATB0 = 0;
    //wait_startup_delay_spi1();
    
    init_counting();
    //init_counting2();
    //config_can(BR_500_KBPS, true, 1, 6, 7);
    start_counting(1000);
    //start_counting2(1);
    
    /*uint8_t to_transmit = 0x01;
    
    sleep(1000);
    complex_comm_spi1(&to_transmit, 1, buf, 6);
    
    while(1);*/
    
    while(1) {
        //sleep(5000);
        _LATB0 = 1;
        wait_startup_delay_spi1();
        _LATB0 = 0;
        wait_startup_delay_spi1();
    }
#else
    config_spi1(SPI_PS1_1_64, SPI_PS2_1_8, 0, 4, 3); // bitrate de 125 kbps
    
    init_counting();
    init_counting2();
    config_can(BR_500_KBPS, true, 1, 6, 7);
    start_counting(1000);
    start_counting2(1);
    
    uint8_t to_transmit = 0x01;
    
    sleep(1000);
    complex_comm_spi1(&to_transmit, 1, buf, 6);
    
    while(1);
#endif
    
    return 0;
}
