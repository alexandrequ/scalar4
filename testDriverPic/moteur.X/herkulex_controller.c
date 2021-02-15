#include "herkulex_controller.h"
#include "herkulex_bus.h"
#include "herkulex_keywords.h"
#include <stddef.h>
#include "timing.h"

void init_servos() {
    configHK(115200);

    sleep(100);
    
    const uint8_t rollback[] = {1, 1};
    sendMessageHK(0xFE, HKL_ROLLBACK, rollback, sizeof(rollback));
    sleep(100);
    sendMessageHK(0xFE, HKL_REBOOT, NULL, 0);
    sleep(500);
}
    
void  set_servo_max_torque(const uint8_t* idList, size_t idListSize, uint16_t max_torque){
    const uint8_t set_max_pwm[] = {HKL_RAM_MAX_PWM_VALUE, 2, (uint8_t)max_torque, (uint8_t)(max_torque>>8)};
    int i;
    for(i = 0; i < idListSize; i++) { // On parcours chaque herkulex dans la liste
        sendMessageHK(idList[i], HKL_RAM_WRITE, set_max_pwm,
                    sizeof(set_max_pwm));
    }
}

void set_servos_pos(const uint8_t* idList, size_t idListSize,
        const uint16_t* pos, uint8_t time, bool wait_pos_reached, bool release_brakes,
        bool set_brakes_at_end) {
    
    // Initialisation of the data to send with the HKL_RAM_WRITE command to toggle brakes
    static const uint8_t torque_release_brakes[] = {HKL_RAM_TORQUE_CONTROL, 1, 0x60};
    static const uint8_t torque_set_brakes[] = {HKL_RAM_TORQUE_CONTROL, 1, 0x40};
    
    static uint8_t go_to_pos[5*256];
    
    uint8_t rxbuf[HKL_MAX_MSG_SIZE];
    
    size_t i;
    for(i = 0; i < idListSize; i++) { // On parcours chaque herkulex dans la liste
        
        if(release_brakes) {
            // On relache les freins
            sendMessageHK(idList[i], HKL_RAM_WRITE, torque_release_brakes,
                    sizeof(torque_release_brakes)); // TODO what if it didn't work ?
        }
        
        // Creation of the data of the ith herkulex for the HKL_I_JOG command
        go_to_pos[5*i] = (uint8_t)(*(pos+i) & 0xFF);
        go_to_pos[5*i+1] = (uint8_t)((*(pos+i) & 0xFF00) >> 8);
        go_to_pos[5*i+2] = 0x04;
        go_to_pos[5*i+3] = idList[i];
        go_to_pos[5*i+4] = time;
        /*go_to_pos[3] = idList[i];
        sendMessageHK(idList[i], HKL_I_JOG, go_to_pos, sizeof(go_to_pos));*/
    }
    // Sending oh the message to set the position of the herkulex
    sendMessageHK(0xFE, HKL_I_JOG, go_to_pos, 5*idListSize);
    
    if(wait_pos_reached) {
        // Initializing a boolean array and a counter
        bool pos_reached[idListSize];
        size_t pos_reached_count = 0;
        for(i = 0; i < idListSize; i++)
            pos_reached[i] = false;
        
        // While there is still a servo that has not reached its position
        while(pos_reached_count != idListSize) {
            // For each servo
            for(i = 0; i < idListSize; i++) {
                // If the servo has reached its position
                if(!pos_reached[i]) {
                    // We ask it its status
                    rxbuf[3] = 0x01;
                    sendMessageHK(idList[i], HKL_STAT, NULL, 0);
                    getAnswerHK(rxbuf);
                    pos_reached[i] = (rxbuf[3] & 0x03) == 0x02; // We analyse the answer
                    if(pos_reached[i]) {
                        pos_reached_count++; // Increments the counter
                        if(set_brakes_at_end) {
                            // Sets the servo in brake mode if required
                            sendMessageHK(idList[i], HKL_RAM_WRITE, torque_set_brakes,
                                    sizeof(torque_set_brakes)); // TODO what if it didn't work ?
                        }
                    }
                }
            }
        }
    }
}

bool servo_pos_reached(const uint8_t* idList, size_t idListSize, bool set_brakes_at_end){
    static const uint8_t torque_set_brakes[] = {HKL_RAM_TORQUE_CONTROL, 1, 0x40};
    bool pos_reached[idListSize];
    uint8_t rxbuf[HKL_MAX_MSG_SIZE];
    size_t pos_reached_count = 0;
    int i;
    
    for(i = 0; i < idListSize; i++)
        pos_reached[i] = false;
    // Unlike the while loop in set_servos_pos(), the previous values of pos_reached
    // are lost between each call. This could cause some bugs.
    // TODO : find a solution (global variable that is reseted with another function ?)
    
    for(i = 0; i < idListSize; i++) {
        rxbuf[3] = 0x01;
        sendMessageHK(idList[i], HKL_STAT, NULL, 0);
        getAnswerHK(rxbuf);
        pos_reached[i] = (rxbuf[3] & 0x03) == 0x02;
        if(pos_reached[i]) {
            pos_reached_count++;
            if(set_brakes_at_end) {
                sendMessageHK(idList[i], HKL_RAM_WRITE, torque_set_brakes,
                        sizeof(torque_set_brakes)); // TODO what if it didn't work ?
            }
        }
    }
    
    return pos_reached_count == idListSize;
}


/*uint16_t get_servo_pos(const uint8_t id){
    uint8_t get_pos[] = {HKL_RAM_ABSOLUTE_POSITION,2};
    sendMessageHK(id,HKL_RAM_READ,get_pos,sizeof(get_pos));
    
    uint8_t rxbuf[HKL_MAX_MSG_SIZE];
    getAnswerHK(rxbuf);
    
    return ((uint16_t)rxbuf[0] + ((uint16_t)rxbuf[1]<<8));
}*/