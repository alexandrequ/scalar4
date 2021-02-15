

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "can.h"
#include "timing.h"

int main(int argc, char** argv) {
    zero_hour();
    
    _PLLPRE = 6;
    _PLLPOST = 0;
    _PLLDIV = 126;
    
    //configuration des leds en sortie et allumage des leds
    _TRISB0=0;
    __builtin_write_OSCCONL(OSCCONL & 0xbf);//deverouillage du peripheral pin select
     
     RPINR18bits.U1RXR=42;    //reception sur pin rp42 (pin 21)
     RPOR4bits.RP43R=0b000001;//transmission uart sur pin rp43 (pin 22)
     
     RPINR26bits.C1RXR=38;    //reception can sur rp38
     RPOR1bits.RP37R=0b001110;//transmission can sur rp37
     
     __builtin_write_OSCCONL(OSCCONL | 0x40);//verouillage
     
     
     uint32_t baudrate = 9600;
     config_uart1(baudrate,UART_CLOSEST);
     
     set_can_mask(CAN_MASK0, 0x7FF);
     enable_can_rx_buffer(CAN_RX8, 0x123, CAN_MASK0);
     
     config_can(BR_500_KBPS, true, 1, 6, 7);
     uint8_t myByte = 0x12;
     
     
     while(1){
        transmit_can(0x100, &myByte, 1,CAN_TX0);
        _LATB0=1;
        sleep(500);
        _LATB0=0;
        sleep(500);
     }
    
     return 0;
}

