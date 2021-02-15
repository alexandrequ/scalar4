/* 
 * File:   integration.h
 * Author: Damien Courteville
 *
 * Created on 17 juin 2018, 19:21
 */

#ifndef INTEGRATION_H
#define	INTEGRATION_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Acquisition_Data{
    
    //Date of the measure
    //long double date;
    
    ///XDI_SampleTimeFine
    uint32_t sample_10kHz_date; // Number of 10kHz ticks since the Xsens reset
    
    ///XDI_DeltaV
    float deltaV[3];
    
    ///XDI_DeltaQ
    float deltaQ[4];
    
    ///XDI_acceleration
    float acc[3]; // (ax,ay,az))
    
    ///XDI_rateofturn (angular velocity)
    float gyr[3]; // (gyrx,gyry,gyrz)
    
    ///XDI_quaternion
    float quat[4]; // (Q1,Q2,Q3,Q4))

    ///XDI_magnetic_field
    float magn[3]; // (Bx,By,Bz)
    
}Acquisition_Data;

extern Acquisition_Data acquisition_data;

/** 
 * @struct Trajecto_Data
 * @brief Structure that hold computed trajectory data
 * 
 * The quaternion represent the rotation to transform the rocket reference frame
 * into a rocket reference frame (aligned with inertial unit).
 * 
 * Terrestrial reference frame axis:
 * - x : Point upward
 * - z : Point in the horizontal direction where the rocket was oriented on launchpad
 * - y : Complete the frame to have a direct reference frame xyz (fr : un repère direct)
 * 
 */
typedef struct Trajecto_Data{
    
    /// Date in the Xsens timeline converted in seconds
    float date;
    
    /// Angular rate in rad/s
    float gyr[3];
    
    /// Rocket quaternion ()
    float quat[4];
    
    /// Rocket position (m)
    float pos[3];
    
    /// Rocket speed (m/s)
    float vel[3];
    
    // Rocket acceleration (m/s^2)
    float acc[3];
    
    // Total impulse of the rocket
    float imp;
    
}Trajecto_Data;

extern Trajecto_Data trajecto_data;

extern float take_off_date;

typedef enum Nominality {HAZARDOUS,NOT_NOMINAL,NOMINAL} Nominality; 

/**
 * @brief Initialize the measurement of gravity and gyros bias.
 * 
 * Initialize the measurement of gravity and gyros bias via a delayed moving average.
 * 
 * @param size Number of measures to use in the averages.
 * @param delay Number of measure the averages will be delayed of.
 * 
 * @pre 'size'+'delay' must be lower than the maximum numer of stored measures defined in delayed_average.h
 */
void initGroundAveraging(int size, int delay);

/**
 * @brief Init the measurement of the acceleration for the inertial propulsion check
 * @param size The number of acceleration measure to average.
 * 
 * @pre 'size' must be lower than the maximum numer of stored measures defined in delayed_average.h
 */
void initAccelerationAveraging(int size);

/**
 * @brief Update the ground measures.
 * 
 * Update the delayed moving average of the gravity, the gyro bias and the acceleration,
 * but if the acceleration is greater than 'IdleAccTreshold', the grzvity measure
 * and the gyro bias measure will not be updated.
 * 
 * @param IdleAccTreshold The acceleration trashold over which the gravity and gyro bias measure will not be updated.
 */
void updateGroundAveraging(float IdleAccTreshold);

/**
 * @brief Save the result of gravity and gyro bias in global variables
 */
void stopGroundAveraging();

/**
 * Set the take_off_date to the current trajecto_data.date
 */
void save_takeoff_date();

/**
 * @brief set 'trajecto_data' to the initial state
 * 
 * @pre get_gravity must have been called at least once
 * 
 * @post the date 'date' is set to the current timestamp()
 * @post The position 'pos' is set to 0
 * @post The velocity 'vel' is set to 0
 * @post The quaternion 'quat' is computed from gravity
 */
void init_trajecto();

/**
 * @brief integrate the angular velocity and acceleration measured
 * 
 * This function compute the new 'trajecto_data' from: \n
 *  - 'pos', 'vel' from the current 'trajecto_data'.
 *  - 'gyr' and 'acc' from 'acquistition_data'.
 *  - The 'date' difference between 'acquisition_data' and 'trajecto_data'.
 * 
 * @post The data of 'trajecto_data' correspond to the situation of the
 * Xsens at the date of the last aquisition
 * @post The 'date' of 'trajecto_data' is equal to 'date' of 'aquisition_data'
 */
void integration();

void incrementation();

/**
 * @brief check is the difference between the predicted orientation and the measured one
 * is within the tolerance.
 * 
 * @param heading_tolerance
 * @param attitude_tolerance_inf
 * @param attitude_tolerance_sup
 * @param critical_heading_tolerance
 * @param critical_attitude_tolerance_inf
 * @param critical_attitude_tolerance_sup
 * @return 
 */
Nominality check_trajecto();

/**
 * 
 * @param freeFallTresh The acceleration under which it is considered that the first engine has stopped thrusting
 * @param propulsionTresh The acceleration above which it is considered that the 2nd engine is thrusting
 * @return 'true' if the second engine thrust has been detected.
 * 
 * @todo This function has never been tested
 */
bool check_propulsion(float freeFallTresh, float propulsionTresh);

#endif	/* INTEGRATION_H */

