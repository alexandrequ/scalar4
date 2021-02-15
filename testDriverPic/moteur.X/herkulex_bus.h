/**
 * @file herkulex_bus.h
 */
//
// Created by ksyy on 04/12/16.
//

#ifndef HERKULEX_SERVO_CONTROLLER_H
#define HERKULEX_SERVO_CONTROLLER_H

#include "herkulex_keywords.h"
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte;

/**
 * @brief Initialize the UART driver for the communication with Herkulex.
 * 
 * @warning The baudrade of the microcontroller must match the baudrate of Herkulex with a precision of 3%.
 * This precision cannot be reached with a CPU frequency of 16MHz.
 * The 115200 baudrate have been tested at 128MHz
 * 
 * @param baudrate The baudrate of the UART bus.
 */
void configHK(const uint32_t baudrate);

/**
 * @brief Send a command to an Herkulex throught the UART bus
 * 
 * @param address The id of the Herkulex
 * @param command The id of the command
 * @param data The data associated with the command
 * @param dataSize The number of bytes of the data
 */
void sendMessageHK( const byte address, const byte command, const byte* data, const int dataSize );


const int getAnswerHK( byte* answer );

void clearSerialHK();

const bool waitingAnswerHK();

#endif //HERKULEX_SERVO_CONTROLLER_H
