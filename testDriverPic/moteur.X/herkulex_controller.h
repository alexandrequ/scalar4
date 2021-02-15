/**
 * @file herkulex_controller.h
 */

/* 
 * File:   herkulex_controller.h
 * Author: d.courteville
 *
 * Created on 23 janvier 2019, 22:21
 */

#ifndef HERKULEX_CONTROLLER_H
#define	HERKULEX_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initialize the Herculex drivers.
 */
void init_servos();

/**
 * @brief Set the position of the Herkulex
 * 
 * The parameter time define the duration of the rotation. The shorter the time, the faster they rotate.
 * If brakes where set at the end of the previous rotation, they must be released
 * before the next rotation.
 * 
 * @param idList The list of ids of the Herkulex to set.
 * @param idListSize The size of 'idList'.
 * @param pos The of the positions to send to each Herkulex.
 * @param time The duration of the rotation of the Herkulex.
 * @param wait_pos_reached If 'true', the function will block until all Herkulex have reached the position.
 * @param release_brakes If 'true', brakes will be realeased before the rotation.
 * @param set_brakes_at_end If 'true', brakes will be set after the rotation.
 */
void set_servos_pos(const uint8_t* idList, size_t idListSize,
        const uint16_t* pos, uint8_t time, bool wait_pos_reached, bool release_brakes,
        bool set_brakes_at_end);

/**
 * @brief Set the maximum torque of the Herkulex
 * 
 * The maximum torque is an integer between 0 and 1023 included. 0 = min, 1023 = max.
 * This is an arbitrary unit, but the maximum torque that the Herkulex can provide
 * is precised in its datasheet.
 * Be aware that even if you set maximum torque to 1023, the real maximum torque
 * is still limited by the power that your power source can provide. 
 * 
 * @param idList The list of ids of the Herkulex to set.
 * @param idListSize The size of 'idList'.
 * @param max_torque The desired maximum torque (0-1023)
 */
void  set_servo_max_torque(const uint8_t* idList, size_t idListSize, uint16_t max_torque);

/**
 * @brief Check if all servo have reached their target position
 * 
 * In other words, this function return 'false' if at least one Herkulex have not
 * reached its target position.
 * 
 * @warning This function has only been tested with only 1 Herkulex.
 * 
 * @warning This function doesn't seems to work if the set_servos_pos() has never been used on the servo before.
 * This is maybe because Herkulex are in a sepcific mode when they are powered on.
 * 
 * @param idList The list of ids of the Herkulex to check.
 * @param idListSize The size of 'idList'.
 * @param set_brakes_at_end If 'true', the servo will be set into brake mode if they reached their position.
 * @return 
 */
bool servo_pos_reached(const uint8_t* idList, size_t idListSize, bool set_brakes_at_end);

#endif	/* HERKULEX_CONTROLLER_H */

