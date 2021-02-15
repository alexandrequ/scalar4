#include <stdio.h>
#include <stdlib.h>
#include "CanInterface.h"
#include <stdbool.h>
#include <linux/can.h>


int main(){
	CanInterface interface;
	interface.init();
	struct can_frame frame;
	frame.can_id = 0x123;
	frame.can_dlc = 0;
	frame.data[0]=10;
	
	interface.send(frame);
	interface.receive(frame);
	cout<<frame.data[0]<<endl;
	return 0;
}