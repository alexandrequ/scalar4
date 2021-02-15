#include "serial.h"
#include <uart.h>
#include <math.h>

typedef struct {
    size_t next;
    size_t to_read;
    bool empty;
    bool overflow;
    uint16_t data[NUM_OF_UART_BUFFERS];
}UartFifo;

static volatile UartFifo uartFifo = {0, 0, true};

/*static uint16_t uart1_rx_fifo[NUM_OF_UART_BUFFERS];
static uint8_t fifo_next = 0;
static uint8_t fifo_to_read = 0;
static bool fifo_empty = true;*/

long double config_uart1(uint32_t baudrate, UartBrMode mode) {

    CloseUART1();
    
    const long double brg_real = (((long double)CPU_FREQ) / 32 / baudrate) - 1; // TODO overflow
    const uint16_t brg_floor = floor(brg_real);
    const uint16_t brg_ceil = ceil(brg_real);
    const long double baudrate_floor = (((long double)CPU_FREQ) / 32 / (brg_floor + 1));
    const long double baudrate_ceil = (((long double)CPU_FREQ) / 32 / (brg_ceil + 1));
    const long double err_floor = fabs(baudrate_floor - baudrate) / baudrate;
    const long double err_ceil = fabs(baudrate_ceil - baudrate) / baudrate;
    
    uint16_t brg = brg_floor;
    long double real_baudrate = baudrate_floor;
    
    switch(mode) {
    case UART_LESS_OR_EQ:
        brg = brg_ceil;
        real_baudrate = baudrate_ceil;
        break;
        
    case UART_GREATER_OR_EQ:
        brg = brg_floor;
        real_baudrate = baudrate_floor;
        break;
        
    case UART_CLOSEST:
        if(err_ceil < err_floor) {
            brg = brg_ceil;
            real_baudrate = baudrate_ceil;
        }
        else {
            brg = brg_floor;
            real_baudrate = baudrate_floor;         
        }
        break;
    }

    ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR3 &
            UART_TX_INT_DIS & UART_TX_INT_PR2);
    
    U1BRG = brg;
    U1MODE = 0b0000100000000000;
    U1STA = 0b1000000100000000;
    U1MODEbits.UARTEN = 0b1;
    U1STAbits.UTXEN = 0b1;
    
    return real_baudrate;
}

void flush_uart1() {
    while(!U1STAbits.TRMT);
}

void transmit_uart1(const uint8_t* bytes, size_t bytes_count, bool block) {
    
    size_t i;
    const uint8_t* cur = bytes;
    for(i = 0; i < bytes_count; i++, cur++) {
        while(U1STAbits.UTXBF); // la FIFO d'envoi est pleine
        U1TXREG = *cur;
    }
    
    if(block)
        flush_uart1();
}

void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt() {
    _U1TXIF = 0;
}

size_t receive_uart1(uint8_t* dest, size_t bytes_count, bool block) {
    
    size_t i = 0;
    uint8_t* cur = dest;
    for(i = 0; i < bytes_count && (!uartFifo.empty || block); i++) {
        while(block && uartFifo.empty); // Si la fonction est bloquante, on attend de recevoir un byte
        __builtin_disi(0x3FFF); // On interdit les interruption
        if(cur != NULL) {
            *cur = uartFifo.data[uartFifo.to_read]; // On ecrit le byte dans le tableau de destination
            cur++;
        }
        // On met le curseur de lecture sur l'élément suivant de la FIFO
        uartFifo.to_read = (uartFifo.to_read == (NUM_OF_UART_BUFFERS-1) ? 0 : uartFifo.to_read+1);
        // Si les curseurs de lecture et d'écriture pointent sur le même element, la FIFO est vide
        uartFifo.empty = uartFifo.next == uartFifo.to_read;
        if(uartFifo.empty) { // Si cet le cas, on en profite pour mettre les curseur à 0
            uartFifo.next = 0;
            uartFifo.to_read = 0;
        }
        __builtin_disi(0); // On autorise les interruptions
    }
    
    return i;
}

size_t new_uart1_received() {
#if 0
    if(uartFifo.empty)
        return 0;
    return ((uartFifo.next+NUM_OF_UART_BUFFERS) - uartFifo.to_read) % NUM_OF_UART_BUFFERS;
#endif
    const size_t apparent_size = ((uartFifo.next+NUM_OF_UART_BUFFERS) - uartFifo.to_read) % NUM_OF_UART_BUFFERS;
    if(apparent_size == 0 && !uartFifo.empty)
        return NUM_OF_UART_BUFFERS;
    return apparent_size;
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt() {
    // Cette interruption est executé quand le U1RIF passe à 1 (Donc quand l'UART 1 reçoit un message). 
    _U1RXIF = 0; // On remet le bit qui à levé l'interruption à 0

    if(uartFifo.empty || uartFifo.next != uartFifo.to_read) { // Si la FIFO n'est pas pleine
        // On écrit le bit reçu dans la FIFO
        uartFifo.data[uartFifo.next] = U1RXREG;
        // On incrémente le curseur d'écriture
        uartFifo.next++;
        if(uartFifo.next == NUM_OF_UART_BUFFERS)
            uartFifo.next = 0;
        uartFifo.empty = false; // On indique que la FIFO n'est pas vide
    }
    else{ // Si la FIFO est pleine
        uartFifo.overflow = true; // On indique que l'on a perdu un byte
    }
    
}

int length(char* text){
    int size=0;
    int i;
    for(i=0;text[i]!='\0' && text[i]!='\n';i++){
        size++;
    }
    return size+1;
}

void Serial_println(char* text){
    int N=length(text);
    uint8_t byte[N];
    int i;
    for(i=0;i<N;i++){
        byte[i]=(uint8_t) text[i];
    }
    transmit_uart1(byte,N,true);
}