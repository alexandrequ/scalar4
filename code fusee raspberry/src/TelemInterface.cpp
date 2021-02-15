#include "TelemInterface.h"

#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define RF_CS_PIN  RPI_V2_GPIO_P1_36 // Slave Select on CE2 so P1 connector pin #36

TelemInterface::TelemInterface(){
	RH_RF95 rf95(RF_CS_PIN);
	base_initialized = false;
	tx_initialized = false;
	rx_initialized = false;
}

bool TelemInterface::isInitialized(){
	return base_initialized;
}

bool TelemInterface::init(float frequency, uint8_t spreding_factor, long signal_bandwith){
	
	if (!rf95.init()){
		return false;
	}
	
	// Adjust Frequency
    rf95.setFrequency( frequency );
    
    //Configuring packets
    rf95.setSignalBandwidth(signal_bandwith);
    rf95.setSpreadingFactor(spreding_factor);
    
    base_initialized = true;
    return true;
}

void TelemInterface::set_transmitter(uint8_t node_id, uint8_t gateway_id, uint8_t power){
	if(!base_initialized){
		#ifdef VERBOSE
		printf("init() must be called successfully before set_transmitter()");
		#endif
		return;
	}
	
	// Setting emission power
	rf95.setTxPower(power, false);
	
	// Setting our Node ID
    rf95.setThisAddress(node_id);
    rf95.setHeaderFrom(node_id);
	
	// Setting destination ID
    rf95.setHeaderTo(gateway_id);
    
    tx_initialized = true;
}

void TelemInterface::set_receiver(uint8_t node_id, bool promiscuous){
	if(!base_initialized){
		#ifdef VERBOSE
		printf("init() must be called successfully before set_transmitter()");
		#endif
		return;
	}
	
	// Setting our Node ID
    rf95.setThisAddress(node_id);
    rf95.setHeaderFrom(node_id);
	
	if(promiscuous)
		rf95.setPromiscuous(true);
	
	// Start listening for message
	rf95.setModeRx();
	
	rx_initialized = true;
}

void TelemInterface::sleep(){
	rf95.sleep();
	tx_initialized = false;
	rx_initialized = false;
}

bool TelemInterface::is_transmitter_avaible(){
	if(!tx_initialized)
		return false;
		
	return rf95.isPacketSent();
}

bool TelemInterface::send(uint8_t tosend[], uint8_t len){
	return rf95.send(tosend, len);
}

bool TelemInterface::send(string message){
	uint8_t len = (uint8_t) message.length();
	uint8_t array[] = malloc(len*sizeof(uint8_t));
	for(int i=0; i<len;i++){
		array[i] = (uint8_t) message[i];
	}
	return rf95.send(array,len);
}

bool TelemInterface::send(vector<uint8_t> vect){
	length = vect.size()
	uint8_t array[length];
	for(int i=0;i<length;i++){
		array[i]=vect[i]
	}
	return rf95.send(array, length);
}

bool TelemInterface::recv(uint8_t buf[], uint8_t* len, uint8_t* from, uint8_t* to, uint8_t* id, uint8_t* flag, int8_t* rssi){
	
	if (rf95.available()) { // Checks if we received a message
	
		// Retreives the informations in the received message header
		// if pointer is not NULL
		if(from != NULL)
			*from = rf95.headerFrom();
		if(to != NULL)
			*to = rf95.headerTo();
		if(id != NULL)
			*id = rf95.headerId();
		if(flag != NULL)
			*flag = rf95.headerFlags();
		if(rssi != NULL)
			*rssi = rf95.lastRssi();
		
		if (rf95.recv(buf, len)) {
			if(rf95.checkLastPayloadCrc())
				return true;
			else
				return false;
		}
	}
	// No message received
	*len=0;
	return false;
}


