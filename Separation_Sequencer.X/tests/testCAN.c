#include <pps.h>
#include <xc.h>

#define NUM_OF_ECAN_BUFFERS 1

unsigned int ecan1MsgBuf[NUM_OF_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_OF_ECAN_BUFFERS * 16)));

int main() {
    PPSUnLock; // FIXME voir si cette ligne est nécessaire
    // Réception CAN sur RP39 (pin n° 16)
    PPSInput(IN_FN_PPS_C1RX, IN_PIN_PPS_RP39);
    // Transmission CAN sur RP40 (pin n° 17)
    PPSOutput(OUT_FN_PPS_C1TX, OUT_PIN_PPS_RP40);

    /* Set up the ECAN1 module to operate at 125 kbps. The ECAN module should be first placed
in configuration mode. */
    C1CTRL1bits.REQOP = 4;
    while (C1CTRL1bits.OPMODE != 4);
    C1CTRL1bits.WIN = 0;

    C1FCTRL = 0x001F; // No FIFO, 4 Buffers

    C1CTRL1bits.CANCKS = 0x0;
    /* Phase Segment 1 time is 7 TQ */
    C1CFG2bits.SEG1PH = 0x6;
    /* Phase Segment 2 time is set to be programmable */
    C1CFG2bits.SEG2PHTS = 0x1;
    /* Phase Segment 2 time is 2 TQ */
    C1CFG2bits.SEG2PH = 0x1;
    /* Propagation Segment time is 6 TQ */
    C1CFG2bits.PRSEG = 0x5;
    /* Bus line is sampled three times at the sample point */
    C1CFG2bits.SAM = 0x1;
    /* Synchronization Jump Width set to 1 TQ */
    C1CFG1bits.SJW = 0x0;
    /* Baud Rate Prescaler bits set to 1:1, (i.e., TQ = (2*1*1)/FCAN) */
    C1CFG1bits.BRP = 0x0;

    /* Assign 4x8word Message Buffers for ECAN1 in device RAM. This example uses DMA0 for TX.
    Refer to 21.8.1 ?DMA Operation for Transmitting Data? for details on DMA channel
    configuration for ECAN transmit. */
    DMA0CONbits.SIZE = 0x0;
    DMA0CONbits.DIR = 0x1;
    DMA0CONbits.AMODE = 0x2;
    DMA0CONbits.MODE = 0x0;
    DMA0REQ = 70;
    DMA0CNT = 7;
    DMA0PAD = (volatile unsigned int) &C1TXD;
    DMA0STAL = (unsigned int) &ecan1MsgBuf;
    DMA0STAH = (unsigned int) &ecan1MsgBuf;
    DMA0CONbits.CHEN = 0x1;

    /* Configure Message Buffer 0 for Transmission and assign priority */
    C1TR01CONbits.TXEN0 = 0x1;
    C1TR01CONbits.TX0PRI = 0x3;
    /* At this point the ECAN1 module is ready to transmit a message. Place the ECAN module in
    Normal mode. */
    C1CTRL1bits.REQOP = 0;
    while (C1CTRL1bits.OPMODE != 0);
    
    unsigned int i = 0;

    while (1) {
        /* Write to message buffer 0 */
        /* CiTRBnSID = 0bxxx1 0010 0011 1100
        IDE = 0b0
        SRR = 0b0
        SID<10:0>= 0b100 1000 1111 */
        ecan1MsgBuf[0][0] = 0x123C;
        /* CiTRBnEID = 0bxxxx 0000 0000 0000
        EID<17:6> = 0b0000 0000 0000 */
        ecan1MsgBuf[0][1] = 0x0000;
        /* CiTRBnDLC = 0b0000 0000 xxx0 1111
        EID<17:6> = 0b000000
        RTR = 0b0
        RB1 = 0b0
        RB0 = 0b0
        DLC = 0b1111 */
        ecan1MsgBuf[0][2] = 0x0008;
        /* Write message data bytes */
        ecan1MsgBuf[0][3] = i;
        ecan1MsgBuf[0][4] = 0xabcd;
        ecan1MsgBuf[0][5] = 0xabcd;
        ecan1MsgBuf[0][6] = 0xabcd;
        /* Request message buffer 0 transmission */
        C1TR01CONbits.TXREQ0 = 0x1;
        /* The following shows an example of how the TXREQ bit can be polled to check if transmission
        is complete. */
        while (C1TR01CONbits.TXREQ0 == 1);
        /* Message was placed successfully on the bus */
        i++;
    }
    while (1);

    return 0;
}
