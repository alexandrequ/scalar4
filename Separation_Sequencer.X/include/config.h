/* 
 * File:   config.h
 * Author: Damien
 *
 * Created on 10 juin 2019, 09:24
 */

#ifndef CONFIG_H
#define	CONFIG_H

// Pin config
#define UMBILICAL _RB7 // Pin 14
#define IO_UMBILICAL _TRISB7
#define LED_RED _LATB14
#define IO_LED_RED _TRISB14
#define LED_GREEN _LATB15
#define IO_LED_GREEN _TRISB15
#define IO_BUZZER _TRISA4
#define BUZZER _LATA4

#define ALL_OPEN_BUTTON _RB10
#define IO_SEP_BUTTON _TRISB10
#define SEP_BUTTON _RB11
#define IO_RECUP_BUTTON _TRISB11


// Timings
#define WAIT_REBONDS_MS 50 // Debouncing duration
#define WAIT_SEPARATION_MS 4000 // Delay between takeoff and separation
#define WAIT_RECUPERATION_MS 4000 // Delay between separation and parachute ejection

#define LED_BLINK_PERIOD 250

// Herculex param
#define SEP_SERVOS_ID               0x0b // ID of the separation servo
 // Duration of servo closing (arbitary unit). The lower it is, the faster it closes.
#define SEP_SERVOS_CLOSE_ROT_TIME   0x16 // Going under 0x16 has no effect
// Duration of servo opening (arbitary unit). The lower it is, the faster it opens.
#define SEP_SERVOS_OPEN_ROT_TIME    0x16 // Going under 0x16 has no effect
#define SEP_SERVOS_OPEN_POS         737 // Open position of separation servo
#define SEP_SERVOS_CLOSED_POS       280 // Closed position of separation servo
#define SEP_SERVO_TORQUE            1023 // Max torque of separation servo. Max 1023

#define RECUP_SERVOS_ID             0x03
 // Duration of servo closing (arbitary unit). The lower it is, the faster it closes.
#define RECUP_SERVOS_CLOSE_ROT_TIME 0x16
// Duration of servo opening (arbitary unit). The lower it is, the faster it opens.
#define RECUP_SERVOS_OPEN_ROT_TIME  0x16
#define RECUP_SERVOS_OPEN_POS       280 // Open position of recuperation servo
#define RECUP_SERVOS_CLOSED_POS     520 // Closed position of recuperation servo
#define RECUP_SERVO_TORQUE          1000 // Max torque of separation servo. Max 1023

//Position of the servos
#define CLOSED_POSITION			280
#define OPEN_ALL_POSITION		500
#define OPEN_SEP_POSITION		0
#endif	/* CONFIG_H */

