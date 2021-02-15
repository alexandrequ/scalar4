/**
 * @file "serial.h"
 * 
 * @brief a library to communicate using uart and to print message on computer
 * 
 * @code
 *   __builtin_write_OSCCONL(OSCCONL & 0xbf);//deverouillage du peripheral pin select
 *    
 *   RPINR18bits.U1RXR=42;    //reception on pin rp42 (pin 21)
 *   RPOR4bits.RP43R=0b000001;//transmission on pin rp43 (pin 22)
 *    
 *    __builtin_write_OSCCONL(OSCCONL | 0x40);//hold of pps
 *    
 *    //configuration of the uart
 *    uint32_t baudrate = 9600;
 *    config_uart1(baudrate,UART_CLOSEST);
 *    
 *    //test de transmission et reception de l'uart
 *    uint8_t byte[2];
 *    receive_uart1(byte,2,true);
 *    transmit_uart1(byte,2,true);
 *    //envoi de message texte
 *    Serial_println("hello");
 *    Serial_println("salut les terriens");
 * @endcode
 */
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/// Taille le la FIFO de l'UART
#define NUM_OF_UART_BUFFERS 64

typedef enum UartBrMode UartBrMode;

enum UartBrMode { UART_CLOSEST, UART_LESS_OR_EQ, UART_GREATER_OR_EQ };

/**
 * @brief Configure and initialize UART1 module.
 * 
 * The Baud Rate passed in parameter may not be compatible with the CPU clock.
 * If this is the case, the Baud Rate will be approximed according to the @p mode
 * parameter (closest, less or equal, greater or equal).
 * 
 * @param baudrate Baud Rate.
 * @param mode The methode of approximation of the Baud Rate.
 * @return The real baud rate that will be used
 */
long double config_uart1(uint32_t baudrate, UartBrMode mode);

/**
 * @brief Transmit a message through UART1 bus.
 * 
 * @param bytes The byte array to transmit.
 * @param bytes_count The number of bytes to transmit.
 * @param block If 'true', the function will block until all bytes are transmitted
 */
void transmit_uart1(const uint8_t* bytes, size_t bytes_count, bool block);

/**
 * @brief Block until UART1 trasmission is finished.
 */
void flush_uart1();

/**
 * @brief Reads a number of bytes from the UART1 reception FIFO.
 * 
 * When a byte is received, it is automatically stored into a FIFO through an
 * interrupt. This function reads @p bytes_count from the FIFO and store them
 * into the @p dest array. If the FIFO does not contain enough bytes and if @p block
 * is set to 'true', the function will block until the required number of bytes 
 * have been received. If @p block is set to 'false', the function will return prematurely
 * if there is not enough bytes in the FIFO.
 * 
 * @param dest The array that will receive the bytes.
 * @param bytes_count The number of bytes to read.
 * @param block If 'true', the function will block until the required number of bytes have been received.
 * @return The number of bytes actually red.
 */
size_t receive_uart1(uint8_t* dest, size_t bytes_count, bool block);

/**
 * @brief Returns the number of bytes stored in the FIFO.
 * 
 * @return The number of bytes stored in the FIFO.
 */
size_t new_uart1_received();

/**
 * @brief a function to calculate the size of a string
 * 
 * @param text a string
 * 
 * @return the size of the string 
 */
int length(char* text);

/**
 * @brief a function to print a text on the console of a computer
 * 
 * To use this function, you have to use a component which allows you to
 * communicate with your computer. Then open the script serial on matlab
 * , launch the program on the pic and then launch the matlab script
 * 
 * @param text the string to display on the console
 * 
 */
void Serial_println(char* text);

#endif
