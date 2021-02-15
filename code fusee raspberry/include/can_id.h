/**
 *@file can_id.h
 * 
 * @brief Provide macros to handle the messages exanged in the bus can.
 * 
 * This file should be included in all projects using the CAN bus. This is where
 * the "protocole" is defined (IDs, content of messages...)
 */

#ifndef CAN_ID_H
#define	CAN_ID_H

// Smaller id = Higher priority

// Raspberry Pi ID
#define RASPI_CAN_ID            10

// Log IDs
#define SEQ_PYRO_LOG_CAN_ID 	0x20 //32
#define SEQ_RECU_LOG_CAN_ID     0x21 //33
#define SEQ_CHUT_LOG_CAN_ID     0x22 //34
#define TRAJ_LOG_CAN_ID         0x23 //35

// Data IDs
#define SEQ_RECU_BARO_CAN_ID    0x30 //48
#define SEQ_CHUT_BARO_CAN_ID    0x31 //49
#define TRAJ_HEADER_CAN_ID      0x32 //50
#define TRAJ_DATA_CAN_ID        0x33 //51

// Generic macros:
// Return the state precised in the log 'can_byte'
#define _get_log_state(can_byte) ((can_byte) & 0x07)
// Return the value of the bit 'pos' in the log 'can_byte'
#define _can_log_flag(can_byte,pos) (((can_byte) & (0x1<<(pos)) )>> (pos))
// Convert the 4 byte from can_data[first] to can_data[first+3] into a float
#define _can_bytes2float(can_data,first) ((float)(*((float*)((can_data)+(first)))))

// Macros used to analyse logs from Ignition_Sequencer:
// This macro return true if there is the log from pyro indicates in ERROR mode
#define _is_separation_error(can_byte) (_get_log_state(can_byte) == 5)
// This macro return true if there is the log from pyro indicates that it is in FLYING mode
#define _is_takeoff_detected(can_byte) (_get_log_state(can_byte) == 2)

// Defines of TRAJ_DATA_CAN message size
#define XSENS_DATA_LEN 		37
#define TRAJ_DATA_LEN 		32

// The macro below are used to decode trajecto data messages from Trajecto_Sequencer
// Gets the date from 'can_data' (float)
#define _get_traj_date(can_data) _can_bytes2float(can_data,0)
// Gets the quaternion from 'can_data' (float[4])
#define _get_traj_quat(can_data) {_can_bytes2float(can_data,16), _can_bytes2float(can_data,20), _can_bytes2float(can_data,24), _can_bytes2float(can_data,28)}
// Gets the position from 'can_data' (float[3])
#define _get_traj_pos(can_data) {_can_bytes2float(can_data,4) , _can_bytes2float(can_data,8) , _can_bytes2float(can_data,12)}

#endif	/* CAN_ID_H */

