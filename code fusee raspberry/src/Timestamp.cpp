#include "Timestamp.h"
#include <stdlib.h>

Timestamp::Timestamp(){
	//Get current time
	gettimeofday(&StartTime,NULL);
}

void Timestamp::set_zero(){
	//Get current time
	gettimeofday(&StartTime,NULL);
}

unsigned long Timestamp::timestamp()
{
	//Declare a variable to store current time
	struct timeval CurrentTime;
	//Get current time
	gettimeofday(&CurrentTime,NULL);
	//Calculate the difference between our start time and the end time
	unsigned long difference = ((CurrentTime.tv_sec - StartTime.tv_sec) * 1000);
	difference += ((CurrentTime.tv_usec - StartTime.tv_usec)/1000);
	//Return the calculated value
	return difference;
}

std::string Timestamp::timestamp_string(){
	return std::to_string(Timestamp::timestamp());
}
