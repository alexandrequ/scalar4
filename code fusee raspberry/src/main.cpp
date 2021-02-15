#include <iostream>
#include <fstream>
#include <string>
#include "CanInterface.h"
#include "FileInterface.h"
#include "TelemInterface.h"
#include "Timestamp.h"
#include "can_id.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include "GPS.h"

using namespace std;

#define CAN_LOG_SIZE 2 // The number of byte a log occupy (address byte + data byte = 2)

#define RF_FREQUENCY  869.5 // Desired frequency in MHz for rf95
#define RF_GATEWAY_ID 1  // ID of the node supposed to receive the packets
#define RF_NODE_ID    10 // Our node ID

#define UMBILICAL RPI_V2_GPIO_P1_15 // Umbilical pin

#define POWEROFF_DELAY 2000000 // Delay after the umbilical is pulled for the Pi to turn off

#define TELEM_STARTUP_DELAY  5000 // Delay in ms after rf95 power on (RBF removed) to wait before firt transmission
#define TELEM_POLLING_DELAY  100 // Period in ms at wich rf95 availability is checked
// Because there is no need to spam rf95 while it is supposed to transmit a packet, 
// so minimum transmit delay are defined below for different packets
#define TELEM_LOG_MIN_DELAY  100 // Minimum transmission delay in ms for a log packet
#define TELEM_DATA_MIN_DELAY 100 // Minimum transmission delay in ms for a data packet

#define GPS_ACTUALISE_DELAY 300

void quat_to_euler(float euler[3], float q[4]){
    // heading = atan2(2*qy*qw-2*qx*qz , 1 - 2*qy^2 - 2*qz^2)
    euler[0] = atan2f(-2*(q[2]*q[3]-q[0]*q[1]) , q[0]*q[0]-q[1]*q[1]-q[2]*q[2]+q[3]*q[3] );
    // attitude = asin(2*qx*qy + 2*qz*qw)
    float sina = 2*(q[3]*q[1]+q[2]*q[0]);
    if(sina >= 1.0)
        euler[1] = M_PI;
    else if(sina <= -1.0)
        euler[1] = - M_PI;
    else
        euler[1] = asinf(sina);

	euler[2] = atan2f(-2*(q[1]*q[2]-q[0]*q[3]) , q[0]*q[0]+q[1]*q[1]-q[2]*q[2]-q[3]*q[3]);
}


int main(int argc, char**argv){
	Timestamp timer;
	CanInterface can;
	ofstream log_file, traj_file, xsens_file, baro_recu_file, baro_chut_file, can_file, gps_file;
	TelemInterface telem;

	// Variable to store the flight state of the rocket
	static enum{GROUND, FLIGHT, OFF, GROUND_CONFIG} state = GROUND_CONFIG;
	// Date of the next check of rf95 availability
	static unsigned long next_telem_check;
	// Date of the take off
	static unsigned long takeoff_date;
	
	// Indicate if data is ready to be transmitted
	uint8_t telem_data_buf[TRAJ_DATA_LEN+1] = {0x02};
	bool telem_data_ready = false;
	
	unsigned long last_gps_actualisation = 0;
	bool telem_gps_ready;

	// Hold the logs to transmit
	// The first byte indicates that this is a log packet
	vector<uint8_t> telem_log_buf[RH_RF95_MAX_MESSAGE_LEN-1] = vector<uint8_t>(0x01);
	
	// Upon reception, data is split in multiple canframes, so we uses this
	// buffer to hold the whole data
	vector<uint8_t> data = vector<uint8_t>();

	// Indicates the type of data being received
	enum{NOT_SET, XSENS_DATA, TRAJ_DATA} receiving_data = NOT_SET;
	
	// Initializes bcm2835
	if (bcm2835_init()){
		// Sets umbilical pin as input
		bcm2835_gpio_fsel(UMBILICAL, BCM2835_GPIO_FSEL_INPT);
		
		if(bcm2835_gpio_lev(UMBILICAL) == 0){
			// If umbilical is plugged, we go to GROUND state
			state = GROUND;
			cout<<"Ground"<<endl;
		}
		else{
			// If umbilical isn't plugged, we go to GROUND_CONFIG state
			state = GROUND_CONFIG;
			cout<<"Ground config"<<endl;
		}
	}
	
	GPS gps = GPS();
	
	// Initializes CAN
	can.init();
	
	// We send an empty massage through CAN to indicates that program has started
	struct can_frame wake_up_frame;
	wake_up_frame.can_id = RASPI_CAN_ID;
	wake_up_frame.can_dlc = 0;
	can.send(wake_up_frame);
	
	log_file.open("./data/log.csv");
	traj_file.open("./data/trajecto.csv");
	xsens_file.open("./data/xsens.csv");
	baro_recu_file.open("./data/baro_recu.csv");
	baro_chut_file.open("./data/baro_chut.csv");
	can_file.open("./data/can.csv");
	gps_file.open("./data/gps.csv");
	
	can_file<<"Date,identifiant,longueur, donnees"<<endl;
	baro_recu_file<<"Date,donnee 1, donnee 2"<<endl;
	baro_chut_file<<"Date,donnee 1, donnee 2"<<endl;
	log_file<<"Date,identifiant,donnee"<<endl;
	xsens_file<<"Date,donnee"<<endl;
	traj_file<<"Date,donnee"<<endl;
	
	cout<<"Initialisation des pins et des fichiers ok"<<endl;
	int i=0;
	
	
	while(true){
		if(timer.timestamp()-last_gps_actualisation > GPS_ACTUALISE_DELAY){
			gps.actualiseData();
			gps_file<<gps<<endl;
			telem_gps_ready=true;
		}

		struct can_frame frame;
		
		if(!telem.isInitialized()){
			if(telem.init(RF_FREQUENCY)){
				
				cout<<"telem initialized"<<endl;
				
				telem.set_transmitter(RF_NODE_ID, RF_GATEWAY_ID, 23);
				if(state == GROUND) 	
					next_telem_check = timer.timestamp() + TELEM_STARTUP_DELAY;
				else 
					
					next_telem_check = timer.timestamp();
			}
		}
		
		switch(state){
			
			case GROUND: 
				if(bcm2835_gpio_lev(UMBILICAL) == 1){ 
					takeoff_date = timer.timestamp();
					state = FLIGHT;
					cout<<"decollage dectete"<<endl;
				}
				break;
				
			case FLIGHT:
				if(timer.timestamp() - takeoff_date > POWEROFF_DELAY){
					telem.sleep();
					bcm2835_close();
					system("poweroff");
					return 0;
				}
				break;
		}

	
		if(can.receive(frame)){ 
			

			can_file<<FileInterface::canframe_string(frame, timer.timestamp())<<endl;

			switch(frame.can_id){
				case TRAJ_LOG_CAN_ID:
					log_file<<timer.timestamp_string() <<","<< frame.can_id<< ","<< frame.data[0]<<endl;
					if(telem_log_buf.size()+2<=RH_RF95_MAX_MESSAGE_LEN){
						telem_log_buf.push_back(frame.can_id);
						telem_log_buf.push_back(frame.data[0]);
					}
					break;
				
				case SEQ_RECU_BARO_CAN_ID:
					baro_recu_file<<timer.timestamp_string() <<","<< _can_bytes2float(frame.data,0) <<","<< _can_bytes2float(frame.data,4)<<endl;
					break;
				
				case SEQ_CHUT_BARO_CAN_ID:
					baro_chut_file<<timer.timestamp_string() <<","<< _can_bytes2float(frame.data,0) <<","<< _can_bytes2float(frame.data,4)<<endl;
					break;
				
				case TRAJ_HEADER_CAN_ID:
					if(frame.data[0] == 1)
						receiving_data = XSENS_DATA;
					else if(frame.data[0] == 2)
						receiving_data = TRAJ_DATA;
					else
						receiving_data = NOT_SET;
					data_index = 0;
					break;
				
				case TRAJ_DATA_CAN_ID:
					if(receiving_data == XSENS_DATA && data.size()+frame.can_dlc <= XSENS_DATA_LEN){
						for(int i = 0; i<frame.can_dlc; i++)
							data.push_back(frame.data[i]);
						
						if(data.size() == XSENS_DATA_LEN){
							xsens_file<<timer.timestamp;
							for(int i=0;i<data.size()-1;i++){
								xsens_file<<data[i]<<",";
							}
							xsens_file<<data[data.size()-1]<<endl;
							receiving_data = NOT_SET;						
						}
					}

					else if(receiving_data == TRAJ_DATA && data.size()+frame.can_dlc <= TRAJ_DATA_LEN){
						for(int i = 0; i<frame.can_dlc; i++)
							data.push_back(frame.data[i]);

						if(data.size() == TRAJ_DATA_LEN){
							float date = _get_traj_date(data);
							float pos[3] = _get_traj_pos(data);
							float quat[4] = _get_traj_quat(data);
							traj_file.write<<date << "," << pos[0] << "," <<pos[1]<< "," << pos[2] << "," << quat[0] <<"," << quat[1] << ","<< quat[2]<<","<<quat[3]<<endl;
							
							for(int i=0; i<TRAJ_DATA_LEN; i++)
								telem_data_buf[1+i] = data[i];
							telem_data_ready = true;
							receiving_data = NOT_SET;
							data_index = 0;
						}
					}
					else{
						receiving_data = NOT_SET;
					}				
					break;
			}
		}
			
		if(telem.isInitialized() && timer.timestamp() > next_telem_check){

			next_telem_check = timer.timestamp() + TELEM_POLLING_DELAY;
			if(telem.is_transmitter_avaible()){

				if(telem_log_buf.size() > 0){
					telem.send(telem_log_buf);
					telem_log_buf = vector<uint8_t>(0x02);
					next_telem_check = timer.timestamp() + TELEM_LOG_MIN_DELAY;
				}

				else if(telem_data_ready){
					telem.send(telem_data_buf, 1+TRAJ_DATA_LEN);
					telem_data_ready = false;
					next_telem_check = timer.timestamp() + TELEM_DATA_MIN_DELAY;
				}

				else if(telem_gps_ready){
					telem.send(gps.to_string());
					telem_gps_ready=false;
					next_telem_check = timer.timestamp() + TELEM_DATA_MIN_DELAY;
				}
			}
		}
	}	
}
