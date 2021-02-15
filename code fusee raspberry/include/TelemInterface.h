/**
  * @file TelemInterface.h
  *
  * @brief cette classe permet d'utiliser la telemetrie. Elle encapsule certaines fonctions de
  * la clase RH_RF95 de radiohead.
  */
#ifndef TELEMINTERFACE_H
#define TELEMINTERFACE_H

#include <stdint.h>
#include "LightCanframe.h"
#include "RH_RF95.h"
#include <stdlib.h>
#include <string>



/**
 * Message aux bizuths: la classe TelemInterface encapsule CERTAINES fonctionnalités
 * de la classe RH_RF95 de RadioHead dont j'avais besoin. Ma documentation 
 * n'explique PAS le fonctionnement de RadioHead ou du module rfm95. Utiliser
 * cette classe ne vous dispense donc pas de lire la documentation de RadioHead 
 * et la datasheet du rfm95 et de comprendre leur fonctionnement, voir même
 * d'écrire votre propre classe.
 */
class TelemInterface
{
	public:

	TelemInterface();
					
	/**
	 * @brief Check if rfm95 module has been initialized.
	 * 
	 * @return true if the method init has been called successfully.
         */
	bool isInitialized();
	
        /**
	 * @brief Initialize rf95 module and set the paramteters common to transmitter and receiver.
	 * 
	 * @param frequency Frequency of the signal in MHz
	 * @param spreding_factor The speading factor of signal symbols, integer between 7 and 12 included.
	 * @param signal_bandwith Bandwidth of the signal in Hz.
	 * 
	 * Spreding factor and Signal Bandwidth have a limited number of possible values.
	 * See rfm95 datasheet for more informations.
	 * A bigger spreading factor and a smaller bandwith lead to an increased range
	 * but a smaller bitrate.
	 * 
	 * The receiver and tranmitter should be initialised with the same parameters.
         */
	bool init(float frequency = 868.00,	uint8_t spreding_factor = 7, long signal_bandwith = 125000);
	
	/***
	 * @brief Configure the rfm95 module into transmitter mode.
	 * 
	 * @param node_id ID of this transmitter module
	 * @param gateway_id ID of the gateway module (the receiver)
	 * @param power emission power in dBm (Integer between 4 and 23 included)
	 * 
	 * The 21, 22 and 23dBm emission power use a special funtionality of
	 * the rfm95. Read the datasheet for more information.
	 * */
	void set_transmitter(uint8_t node_id, uint8_t gateway_id, uint8_t power);
	
        /**
	 * @brief Configure the rfm95 module into receiver mode.
	 * 
	 * @param node_id ID of this receiver module
	 * @param promiscuous If false, message with a gateway id different from node_id are ignored.
         */
	void set_receiver(uint8_t node_id, bool promiscuous = false);
	
        /**
	 * @brief Configure the rfm95 module into sleep mode
	 * 
	 * The sleep mode has a very low power consumption.
         */
	void sleep();
	
	/**
	 * @brief Check if the rfm95 module is ready to send a message.
	 * 
	 * @return true if the module is ready to transmit, false else.
	 * 
	 * Here is some of the reasons a false is returned:
	 *  - The module is already transmitting a message.
	 *  - The module is not configured as a transmitter.
	 * 
	 * @warning Do not call this function in a loop like a savage bizuth,
	 * if the module is solicited too frequently, it will interfere with
	 * the transmission. You shall implement a delay between each availability
	 * check. 100ms is enough, but a smaller delay is probably OK. 
	 * 
         */
	bool is_transmitter_avaible();
	
     /**
	 * @brief Send message through rfm95.
	 * 
	 * @param tosend The bytes to send.
	 * @param size The size of the message.
	 * 
	 * @return true if the transmission has started successfully, false else.
         */
	bool send(uint8_t tosend[], uint8_t size);
	
        /**
	 * @brief Tries to receive a message through rfm95 module.
	 * 
	 * @return true if a valid message has been copied into 'buf'
	 * 
	 * 'len' is a pointer to the number of bytes to read. After reception,
	 * the value pointed by 'len' is set to the number of data bytes effectively
	 * received. The received bytes are stored in 'buf'. The other optional
	 * parameters are pointers to variables that will receive the message
	 * metadata contained in the message's header:
	 * - from: the source ID
	 * - to: the destination ID
	 * - id: ???
	 * - flag: flag bits associated to the message
	 * - rssi: signal-to-noise ratio of the received message
	 * 
	 * @warning 'len'=0 does not mean that no message is red/received, it means
	 * that no data bytes are red/received. The header is still red with its
	 * metadata.
	 *  
         */
	bool recv(uint8_t buf[], uint8_t* len,
				uint8_t* from = NULL,
				uint8_t* to = NULL,
				uint8_t* id = NULL,
				uint8_t* flag = NULL,
				int8_t* rssi = NULL);
	
	bool send(string message);
	
	bool send(vector<uint8_t> array);
	
	private:
	RH_RF95 rf95;
	
	bool base_initialized;
	
	bool tx_initialized;
	
	bool rx_initialized;
};

#endif
