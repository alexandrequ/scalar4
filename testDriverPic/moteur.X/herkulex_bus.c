//
// Created by ksyy on 04/12/16.
//

#include "herkulex_bus.h"
#include "serial.h"
#include "timing.h"
#include <stddef.h>

static byte _message[ HKL_MAX_MSG_SIZE ];
static byte _answer[ HKL_MAX_MSG_SIZE ];

//static const size_t ANSWER_QUEUE_SIZE = 10;
#define ANSWER_QUEUE_SIZE ((size_t) 10)
static uint8_t _answerSizes[ ANSWER_QUEUE_SIZE ]; // The size of the queued answers
static size_t _nextAnswerRead, _nextAnswerWrite;

static void addChecksumsHK( byte* message, const int messageSize );
static bool checksumsHK( byte* message, const int messageSize );

#if 0
ServoController::ServoController( decltype( Serial1 )& serialInterface )
	: _serialInterface( serialInterface )
{
//	clearSerial();
}

void ServoController::begin( const int baudrate )
{
	_serialInterface.end();
	_serialInterface.begin( baudrate );
}
#endif

void configHK(const uint32_t baudrate) {
    config_uart1(baudrate, UART_CLOSEST);
}

void sendMessageHK( const byte address, const byte command, const byte* data, const int dataSize )
{
	// Eliminate speed control commands
	if( command == 0x05 || command == 0x06 ) {
        int i;
		for( i = ( command == 0x05 ? 2 : 3 ) ; i < dataSize ; i += command == 0x05 ? 5 : 4 )
			if( data[ i ] & 0x02 )
				return;
	}
	
	// Header
	_message[ 0 ] = _message[ 1 ] = HKL_HEADER;
	// Packet Size
	_message[ 2 ] = 7 + dataSize;
	// Address
	_message[ 3 ] = address;
	// Command
	_message[ 4 ] = command;
	// Data
    int i;
	for( i = 0 ; i < dataSize ; ++i )
		_message[ 7 + i ] = data[ i ];
	
	// Checksums
	addChecksumsHK( _message, dataSize + 7 );
	
	// Sending
	//_serialInterface.write( _message, 7 + dataSize );
    transmit_uart1(_message, 7 + dataSize, false);
	
	if( command == HKL_EEP_READ || command == HKL_RAM_READ || command == HKL_STAT )
	{
		// If _nextAnswerWrite points to a used value, something wrong happened (no response)
		//_serialInterface.flush();
        flush_uart1();
		_answerSizes[ _nextAnswerWrite++ ] = command == HKL_STAT ? 9 : data[ 1 ] + 11;
		if( _nextAnswerWrite == ANSWER_QUEUE_SIZE )
			_nextAnswerWrite = 0;
	}
}

static void addChecksumsHK( byte* message, const int messageSize )
{
	// The cast eliminates a narrowing warning (^ convert to int).
	byte checksum1 = ( (byte)( ( message[ 2 ] ^ message[ 3 ] ) ^ message[ 4 ] ) );
	
    int i;
	for( i = 7; i < messageSize ; ++i )
		checksum1 ^= message[ i ];
		
	message[ 5 ] = checksum1 &= HKL_CHECKSUM;
	message[ 6 ] = ~checksum1 & HKL_CHECKSUM;
}

static bool checksumsHK( byte* message, const int messageSize )
{
	byte checksum1 = ( message[ 5 ] ), checksum2 = ( message[ 6 ] );
	
	addChecksumsHK( message, messageSize );
	
	return ( checksum1 == message[ 5 ] ) && ( checksum2 == message[ 6 ] );
}

/*const int available()
{
	//return _serialInterface.available();
    return new_uart1_received();
}*/

const int getAnswerHK( byte* answer )
{
	// If we do not expect a reading...
	if( _answerSizes[ _nextAnswerRead ] == 0 )
		return 0;
	
	// Awaiting message reception...
    sleep(5);
	
	uint8_t answerSize = _answerSizes[ _nextAnswerRead ];
	_answerSizes[ _nextAnswerRead++ ] = 0;
	if( _nextAnswerRead == ANSWER_QUEUE_SIZE )
		_nextAnswerRead = 0;
	
	// If the message has not been received after 5ms, something is wrong, we abort this reading.
	if( new_uart1_received() < answerSize )
		return 0;
	
	
	// Reading
	/*for( size_t i{ 0 } ; i < answerSize ; ++i )
		_answer[ i ] = _serialInterface.read();*/
    receive_uart1(_answer, answerSize, true);
	
	// Checks the headers and the size of the message
	if( _answer[ 0 ] != HKL_HEADER || _answer[ 1 ] != HKL_HEADER || _answer[ 2 ] != answerSize )
		return 0;
	
	
	// Checks the checksums
	if( !checksumsHK( _answer, _answer[ 2 ] ) )
		return 0;
	
	// Gets the necessary information
	answer[ 0 ] = _answer[ 3 ];
	answer[ 1 ] = _answer[ 4 ];
    int count;
	for( count = 7 ; count < _answer[ 2 ] ; ++count )
		answer[ count - 5 ] = _answer[ count ];
	
	return _answer[ 2 ] - 5;
}

void clearSerialHK()
{
	_nextAnswerRead = _nextAnswerWrite = 0;
	
    size_t i;
	for( i = 0 ; i < ANSWER_QUEUE_SIZE ; ++i )
		_answerSizes[ i ] = 0;
	
	while( new_uart1_received() )
		receive_uart1(NULL, 1, true);
}

const bool waitingAnswerHK()
{
	return _answerSizes[ _nextAnswerRead ] != 0;
}

