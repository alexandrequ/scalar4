/* 
 * File:   config.h
 * Author: d.courteville
 *
 * Created on 24 mars 2019, 21:21
 */

#ifndef CONFIG_H
#define	CONFIG_H
#include "can_id.h"

#define CARTE_PROTO 0

// Pinout
#if CARTE_PROTO
    #define CTS _LATB9  // Pin 18
        #define IO_CTS _TRISB9
    #define nRST _LATB11 // Pin 22
        #define IO_nRST _TRISB11
    #define ENABLE_IGNITION _LATB13 // pin 24
        #define IO_ENABLE_IGNITION _TRISB13
    #define UMBILICAL _RB12 // Pin 23
        #define IO_UMBILICAL _TRISB12
#else
    // Pin CTS of Xsens. Unsused.
    #define CTS _LATA0  // Pin 2
        #define IO_CTS _TRISA0
    // No Reset pin of Xsens
    #define nRST _LATB14 // Pin 25
        #define IO_nRST _TRISB14
    // Pin connected with Ignition Sequenceur to transmit 'trajecto ok' signal
    #define ENABLE_IGNITION _LATA4 // pin 22 // 
        #define IO_ENABLE_IGNITION _TRISA4
    #define UMBILICAL _RB12 // Pin 23
        #define IO_UMBILICAL _TRISB12
#endif

#define LED1 _LATB0
    #define IO_LED1 _TRISB0
#define LED2 _LATB1
    #define IO_LED2 _TRISB1

#define LED_BLINK_PERIOD_GROUND_MS 1000
#define LED_BLINK_PERIOD_FLYING_MS 250

// Software Parameters
#define UMBILICAL_DEBOUNCING_DELAY 5 // Durï¿½e pendant laquelle l'ombillical doit ï¿½tre dï¿½branchï¿½ pour detecter le dï¿½collage
#define XSENS_RX_TIMEOUT 500 // Temps d'attente de mesures envoyées par la Xsens

#define SLEEP_DATE_MS 2000000 // Time after takeoff at which the device is put into sleep mode

// Number of values used to compute the moving average of acceleration and rotation speed ground measures
#define GROUND_AVERAGING_SIZE 400
// Number of measures the moving average is delayed
#define GROUND_AVERAGING_DELAY 100
// Number of values used to compute the moving average of acceleration on lauchpad and during the flight
#define ACC_AVERAGING_SIZE 50

#define LOG_CAN_ID TRAJ_LOG_CAN_ID // ID of log messages
#define HEADER_CAN_ID TRAJ_HEADER_CAN_ID // ID of header messages
#define DATA_CAN_ID TRAJ_DATA_CAN_ID // ID of data messages
#define LOG_PERIOD 1000

#define PYRO_CAN_ID SEQ_PYRO_LOG_CAN_ID // ID of logs from Ignition Sequencer


// Flight Parameters
#define FREE_FALL_ACC_TRESH 1 // acceleration en m/s^2 en deça de laquelle on considere que la fusee est en chute libre
#define PROPULSION_ACC_TRESH 50 // acceleration en m/s^2 au dela de laquelle on considere que le moteur est allumé
#define IDLE_ACCELERATION_TRESHOLD 10 // acceleration en m/s^2 en deça de laquelle on considere que la fusee est posée au sol

#define MAX_ELEVATION_RAD   1.40 // 80°
#define MIN_ELEVATION_RAD   0.78 // 45°
#define MAX_HEADING_RAD     0.78 // 45°
#define MIN_HEADING_RAD    -0.78 //-45°

#define MAX_CRIT_ELEVATION_RAD   1.57 // 90°
#define MIN_CRIT_ELEVATION_RAD   0.50 // 29°
#define MAX_CRIT_HEADING_RAD     1.00 // 57.3°
#define MIN_CRIT_HEADING_RAD    -1.00 //-57.3°

#endif	/* CONFIG_H */

