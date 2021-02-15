#include "can.h"
#include "timing.h"
#include <xc.h>
//#include <pps.h>

#define MAX_NUM_OF_ECAN_BUFFERS 32
#define NUM_OF_ECAN_BUFFERS 16 // Nombre de buffers CAN

#if NUM_OF_ECAN_BUFFERS > MAX_NUM_OF_ECAN_BUFFERS
#error "You can't have more than 32 buffers per ECAN module."
#elif NUM_OF_ECAN_BUFFERS <= 0
#error "You have a sense of humor haven't you ?"
#endif

// Définition des variables globales servant de buffers de transmission/réception
// pour le bus CAN
uint16_t ecan1MsgBuf[NUM_OF_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_OF_ECAN_BUFFERS * 16)));

bool reception_enabled = false;

const uint16_t BUFFER_BIT_MASK[MAX_NUM_OF_ECAN_BUFFERS] = { 
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000};

uint16_t* const C1RXMnSID[3] = {
    (uint16_t* const)&C1RXM0SID, (uint16_t* const)&C1RXM1SID,
    (uint16_t* const)&C1RXM2SID
};

uint16_t* const C1RXFnSID[15] = {
    (uint16_t* const)&C1RXF0SID, (uint16_t* const)&C1RXF1SID,
    (uint16_t* const)&C1RXF2SID, (uint16_t* const)&C1RXF3SID,
    (uint16_t* const)&C1RXF4SID, (uint16_t* const)&C1RXF5SID,
    (uint16_t* const)&C1RXF6SID, (uint16_t* const)&C1RXF7SID,
    (uint16_t* const)&C1RXF8SID, (uint16_t* const)&C1RXF9SID,
    (uint16_t* const)&C1RXF10SID, (uint16_t* const)&C1RXF11SID,
    (uint16_t* const)&C1RXF12SID, (uint16_t* const)&C1RXF13SID,
    (uint16_t* const)&C1RXF14SID
};

void set_can_mask(Canmask mask_sel, uint16_t mask_value) {
    
    // Passage du module CAN dans le mode de configuration
    C1CTRL1bits.REQOP = 4;
    while (C1CTRL1bits.OPMODE != 4);
    
    C1CTRL1bits.WIN = 1;
    //C1RXM0SIDbits.SID = mask_value;
    //C1RXM0SIDbits.MIDE = 0x1;
    //C1RXM0SID = ((0x7FF & mask_value) << 5) | (0x1 << 3);
    *C1RXMnSID[mask_sel] = ((0x7FF & mask_value) << 5) | (0x1 << 3);
    C1CTRL1bits.WIN = 0;
}

void enable_can_rx_buffer(Canrxbuf buffer, uint16_t sid_filter,
        Canmask mask_sel) {
    
    unsigned int i;
    for(i = 0; i < 8; i++)
        ecan1MsgBuf[buffer][i] = 0x0000;
    
    // Passage du module CAN dans le mode de configuration
    C1CTRL1bits.REQOP = 4;
    while (C1CTRL1bits.OPMODE != 4);
    
    C1CTRL1bits.WIN = 1;
    
    C1FMSKSEL2 &= ~(0b11 << (2*(buffer-8)));
    C1FMSKSEL2 |= (mask_sel << (2*(buffer-8)));
    //C1FMSKSEL2bits.F8MSK = 0x0;
    //C1RXF8SIDbits.SID = 0x123;
    *C1RXFnSID[buffer] = (0x7FF & sid_filter) << 5;
    
    //C1RXF8SIDbits.EXIDE = 0x0;
    
    //C1BUFPNT3bits.F8BP = 8;
    if(buffer < 4) {
        C1BUFPNT1 &= ~(0xF << (4*buffer));
        C1BUFPNT1 |= buffer << (4*buffer);
    }
    else if(buffer < 8) {
        C1BUFPNT2 &= ~(0xF << (4*(buffer-4)));
        C1BUFPNT2 |= buffer << (4*(buffer-4));
    }
    else if(buffer < 11) {
        C1BUFPNT3 &= ~(0xF << (4*(buffer-8)));
        C1BUFPNT3 |= buffer << (4*(buffer-8));
    }
    else {
        C1BUFPNT4 &= ~(0xF << (4*(buffer-12)));
        C1BUFPNT4 |= buffer << (4*(buffer-12));
    }
    
    //C1FEN1bits.FLTEN8 = 0x1;
    C1FEN1 |= BUFFER_BIT_MASK[buffer];
    
    C1CTRL1bits.WIN = 0;
    
    reception_enabled = true;
}

void config_can(Bitrate br, bool trisample, unsigned short sjw, 
        unsigned short propseg, unsigned short seg1) {

    // Passage du module CAN dans le mode de configuration
    C1CTRL1bits.REQOP = 4;
    while (C1CTRL1bits.OPMODE != 4);
    // Accès aux registres de transmission et de configuration des buffers
    C1CTRL1bits.WIN = 0;

    C1FCTRL = 0x801F; // Pas de FIFO, 16 buffers
    
    // Calcul de seg2 à partir des autres paramètres
    const unsigned short seg2 = NTQ - (1 + propseg + seg1);

#ifdef DOUBLE_FCAN
    C1CTRL1bits.CANCKS = 0x1; // FCAN = FOSC
#else
    C1CTRL1bits.CANCKS = 0x0; // FCAN = FOSC/2
#endif
    C1CFG2bits.SEG1PH = (seg1 & 0b111) - 1; // Configuration de seg1
    C1CFG2bits.SEG2PHTS = 0x1; // Programmation de seg2 autorisée
    C1CFG2bits.SEG2PH = (seg2 & 0b111) - 1; // Configuration de seg2
    C1CFG2bits.PRSEG = (propseg & 0b111) - 1; // Configuration segment de propagation
    C1CFG2bits.SAM = (trisample & 0b1); // Configuration du "triple-sampling"
    C1CFG1bits.SJW = (sjw & 0b11) - 1; // Configuration du sjw
    C1CFG1bits.BRP = br; // Configuration du Baud Rate Prescaler

    // Configuration du DMA : composant en charge de transférer automatiquement
    // les données entre les buffers ecan1MsgBuf et le module CAN
    
    DMA0CONbits.SIZE = 0x0; // Tailles des buffers exprimées en words (=2 octets)
    DMA0CONbits.DIR = 0x1; // Sens du transfert : du buffer vers le module CAN
    DMA0CONbits.AMODE = 0x2; // Mode d'adressage indirect
    DMA0CONbits.MODE = 0x0; // Mode continu et sans ping-pong
    DMA0REQ = 0x46; // Périphérique vers lequel transférer : module CAN
    DMA0CNT = 7; // Un buffer est entièrement transmis au bout de 8 transferts
    DMA0PAD = (volatile unsigned int) &C1TXD; // Destination : registre données CAN
    DMA0STAL = (unsigned int) &ecan1MsgBuf; // Source : ecan1MsgBuf
    DMA0STAH = (unsigned int) &ecan1MsgBuf; // Source : ecan1MsgBuf
    DMA0CONbits.CHEN = 0x1; // Activation du DMA
    
    /* Configure Message Buffer 0 for Transmission and assign priority */
    C1TR01CONbits.TXEN0 = 0x1; // Buffer 0 utilisé en transmission
    C1TR01CONbits.TX0PRI = 0x3; // Buffer  à priorité maximale
    /* Configure Message Buffer 1 for Transmission and assign priority */
    C1TR01CONbits.TXEN1 = 0x1; // Buffer 1 utilisé en transmission
    C1TR01CONbits.TX1PRI = 0x3; // Buffer 1 à priorité maximale
    /* Configure Message Buffer 2 for Transmission and assign priority */
    C1TR23CONbits.TXEN2 = 0x1; // Buffer 2 utilisé en transmission
    C1TR23CONbits.TX2PRI = 0x3; // Buffer 2 à priorité maximale
    /* Configure Message Buffer 3 for Transmission and assign priority */
    C1TR23CONbits.TXEN3 = 0x1; // Buffer 3 utilisé en transmission
    C1TR23CONbits.TX3PRI = 0x3; // Buffer 3 à priorité maximale
    /* Configure Message Buffer 4 for Transmission and assign priority */
    C1TR45CONbits.TXEN4 = 0x1; // Buffer 4 utilisé en transmission
    C1TR45CONbits.TX4PRI = 0x3; // Buffer 4 à priorité maximale
    /* Configure Message Buffer 5 for Transmission and assign priority */
    C1TR45CONbits.TXEN5 = 0x1; // Buffer 5 utilisé en transmission
    C1TR45CONbits.TX5PRI = 0x3; // Buffer 5 à priorité maximale
    /* Configure Message Buffer 6 for Transmission and assign priority */
    C1TR67CONbits.TXEN6 = 0x1; // Buffer 6 utilisé en transmission
    C1TR67CONbits.TX6PRI = 0x3; // Buffer 6 à priorité maximale
    /* Configure Message Buffer 7 for Transmission and assign priority */
    C1TR67CONbits.TXEN7 = 0x1; // Buffer 7 utilisé en transmission
    C1TR67CONbits.TX7PRI = 0x3; // Buffer 7 à priorité maximale
    
    if(reception_enabled) {
        DMA1CONbits.SIZE = 0x0; // Tailles des buffers exprimées en words (=2 octets)
        DMA1CONbits.DIR = 0x0; // Sens du transfert : du module CAN vers le buffer
        DMA1CONbits.AMODE = 0x2; // Mode d'adressage indirect
        DMA1CONbits.MODE = 0x0; // Mode continu et sans ping-pong
        DMA1REQ = 0x22; // Périphérique duquel transférer : module CAN
        DMA1CNT = 7; // Un buffer est entièrement rempli au bout de 8 transferts
        DMA1PAD = (volatile unsigned int) &C1RXD; // Source : registre données CAN
        DMA1STAL = (unsigned int) &ecan1MsgBuf; // Destination : ecan1MsgBuf
        DMA1STAH = (unsigned int) &ecan1MsgBuf; // Destination : ecan1MsgBuf
        DMA1CONbits.CHEN = 0x1; // Activation du DMA
        
        /*C1CTRL1bits.WIN = 1;
        C1RXM0SIDbits.SID = filters_mask;
        C1RXM0SIDbits.MIDE = 0x1;
        C1CTRL1bits.WIN = 0;*/
    }
    
    // Passage du module CAN dans le mode normal
    C1CTRL1bits.REQOP = 0;
    while (C1CTRL1bits.OPMODE != 0);
}

void transmit_can(uint16_t id, const uint8_t* databytes, unsigned short bytes_count, Cantxbuf buffer){
    switch(buffer){ // Interrompt la transmission précédente
        case CAN_TX0: C1TR01CONbits.TXREQ0 = 0; break;
        case CAN_TX1: C1TR01CONbits.TXREQ1 = 0; break;
        case CAN_TX2: C1TR23CONbits.TXREQ2 = 0; break;
        case CAN_TX3: C1TR23CONbits.TXREQ3 = 0; break;
        case CAN_TX4: C1TR45CONbits.TXREQ4 = 0; break;
        case CAN_TX5: C1TR45CONbits.TXREQ5 = 0; break;
        case CAN_TX6: C1TR67CONbits.TXREQ6 = 0; break;
        case CAN_TX7: C1TR67CONbits.TXREQ7 = 0; break;
    }
    
    /* 1er mot de la trame standard, du bit le plus fort au plus faible :
     * 3 bits indifférents, mis à 0 ici
     * 11 bits d'identifiant de trame
     * 1 bit RTR à 0 pour une trame standard
     * 1 bit IDE à 0 pour une trame standard */
    ecan1MsgBuf[buffer][0] = id << 2;
    
    // Le 2ème mot sert à l'identifiant de trame étendue, on le met à 0 ici
    ecan1MsgBuf[buffer][1] = 0x0000;
    
    /* 3ème mot de la trame standard, du bit le plus fort au plus faible :
     * 11 bits utilisés pour les trames étendues, mis à 0 ici
     * 1 bit RB0 à 0 pour une trame standard 
     * 4 bits DLC encodant le nombre d'octets de données */
    ecan1MsgBuf[buffer][2] = bytes_count;
   
    // Les 4 mots suivants contiennent les données à transmettre
    unsigned short i;
    uint8_t* p = (uint8_t*)(&(ecan1MsgBuf[buffer][3]));
    for(i = 0; i < bytes_count; i++)
        *(p+i) = *(databytes+i);

    switch(buffer){ // On initie la transmission
        case CAN_TX0: C1TR01CONbits.TXREQ0 = 1; break;
        case CAN_TX1: C1TR01CONbits.TXREQ1 = 1; break;
        case CAN_TX2: C1TR23CONbits.TXREQ2 = 1; break;
        case CAN_TX3: C1TR23CONbits.TXREQ3 = 1; break;
        case CAN_TX4: C1TR45CONbits.TXREQ4 = 1; break;
        case CAN_TX5: C1TR45CONbits.TXREQ5 = 1; break;
        case CAN_TX6: C1TR67CONbits.TXREQ6 = 1; break;
        case CAN_TX7: C1TR67CONbits.TXREQ7 = 1; break;
    }
}

void transmit_canframe(Canframe frame, Cantxbuf buffer) {
    transmit_can(frame.id, frame.databytes, frame.bytes_count, buffer);
}

bool is_buffer_available(Cantxbuf buffer) {
    switch(buffer){
        case CAN_TX0: return C1TR01CONbits.TXREQ0 == 0;
        case CAN_TX1: return C1TR01CONbits.TXREQ1 == 0;
        case CAN_TX2: return C1TR23CONbits.TXREQ2 == 0;
        case CAN_TX3: return C1TR23CONbits.TXREQ3 == 0;
        case CAN_TX4: return C1TR45CONbits.TXREQ4 == 0;
        case CAN_TX5: return C1TR45CONbits.TXREQ5 == 0;
        case CAN_TX6: return C1TR67CONbits.TXREQ6 == 0;
        case CAN_TX7: return C1TR67CONbits.TXREQ7 == 0;
    }
    return false;
}

Canframe receive_can(Canrxbuf buffer, bool wait_reception) {
    Canframe frame;

    while(wait_reception && (C1RXFUL1 & BUFFER_BIT_MASK[buffer]) == 0);
    
    frame.id = (ecan1MsgBuf[buffer][0] >> 2) & 0x07FF;
    frame.bytes_count = ecan1MsgBuf[buffer][2] & 0x000F;
    frame.databytes = (uint8_t*)(&ecan1MsgBuf[buffer][3]);
    
    C1RXFUL1 &= ~BUFFER_BIT_MASK[buffer];
    
    return frame;
}

bool new_canframe_received(Canrxbuf buffer) {
    return (C1RXFUL1 & BUFFER_BIT_MASK[buffer]) != 0;
}