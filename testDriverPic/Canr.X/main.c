

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "can.h"

int main(int argc, char** argv) {
    __builtin_write_OSCCONL(OSCCONL & 0xbf);//deverouillage du peripheral pin select
     
     RPINR26bits.C1RXR=41;    //reception can sur rp41
     RPOR3bits.RP40R=0b001110;//transmission can sur rp40
     
     __builtin_write_OSCCONL(OSCCONL | 0x40);//verouillage

     
 
     set_can_mask(CAN_MASK0, 0x7FF);
     enable_can_rx_buffer(CAN_RX8, 0x123, CAN_MASK0);

     config_can(BR_125_KBPS, true, 1, 6, 7);
     uint8_t myByte = 0x12;
     Canframe donnee={0x100,&myByte,1};
     transmit_canframe(donnee, CAN_TX0);
    
     return 0;
}

