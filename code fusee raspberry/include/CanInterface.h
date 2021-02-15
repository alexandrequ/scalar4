#ifndef CANINTERFACE_H
#define CANINTERFACE_H

#include <stdbool.h>
#include <linux/can.h>

/**
 * @class CanInterface
 * @brief Simple class that manage CAN transmission and reception
 * 
 * @see https://www.kernel.org/doc/html/v4.18/networking/can.html for more 
 * informations about 'struct can_frame'
 * */
class CanInterface
{
	public:
	CanInterface();
	
	/**
	 * @brief Initialize the CAN interface
	 * */
	void init();
	
	/**
	 * @brief Try to receive a CAN message
	 * 
	 * @param frame A reference to the can_frame structure that will receive the message.
	 * 
	 * @return 'true' if a valid message has been received, 'false' else.
	 * 
	 * @todo I don't know what happen to the reference 'frame' if the function return false.
	 * @todo I don't know what happen if 2 message have been received before we call this function.
	 * (i.e. does a new message override the last one, or are the messages put into a FIFO)
	 * */
	bool receive(struct can_frame& frame);
	
	/**
	 * @brief Send a message through CAN bus.
	 * 
	 * @param frame A reference to the can_frame structure that will be sent
	 * 
	 * @todo What happen if the function is called before the previous message is sent ?
	 * */
	bool send(struct can_frame& frame);
	
	private:
	
	/// This is the id of the file representing the CAN socket.
	int cansocket;
};

#endif
