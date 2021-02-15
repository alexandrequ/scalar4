/**
 * @file config.h
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include "can_id.h"

// Pin config
#define LED1 _LATB0
    #define IO_LED1 _TRISB0
#define LED2 _LATB1
    #define IO_LED2 _TRISB1
#define UMBILICAL _RB12 // Pin 23
    #define IO_UMBILICAL _TRISB12
// Fin De Course separation 1
#define FDC1 _RB8
    #define IO_FDC1 _TRISB8
// Fin De Course separation 2
#define FDC2 _RB9
    #define IO_FDC2 _TRISB9
// Fin De Course coiffe
#define FDC_COIFFE _RB13
    #define IO_FDC_COIFFE _TRISB13
// The pin linked with Trajecto Sequencer
#define LOG_TRAJ _RA4
    #define IO_LOG_TRAJ _TRISA4

#ifdef BUZZER_MODE
// In buzzer mode, nIGNITION is replaced by nBUZZER, so we define it with an unused variable
#define nIGNITION ignitition_placeholder
    #define IO_IGNITION tris_ignitition_placeholder
// The pin that make the buzzer ring
#define nBUZZER _LATB15
    #define IO_BUZZER _TRISB15

#define RECU_BUFFER CAN_RX8 // CAN buffer where recuperation sequencer's messages are stored
#define CHUT_BUFFER CAN_RX9 // CAN buffer where cansat sequencer's messages are stored
#define TRAJ_BUFFER CAN_RX10 // CAN buffer where trajecto sequencer's messages are stored
#else
// The pin that transmit ignition order
#define nIGNITION _LATB15 // Old : RB10
    #define IO_IGNITION _TRISB15
#endif

// Parameters
// ADC value threshold of inductive sensor:
// value >  INDU_TRESHOLD --> not separated
// value <= INDU_TRESHOLD --> separated
#define INDU_TRESHOLD 512
#define WAIT_IGNITING_MS 4000 // Delay before the begining of ignition window (ms)
#define IGNITION_WINDOW_MS 16000 // Duration of ignition window (ms)
#define SEP_IGNITION_DELAY_MS 1000 // How long the rocke should be separated to allow ignition (ms)
#define SLEEP_DATE_MS 2000000 // Date at which the device is put into sleep mode (ms)
#define WAIT_REBONDS_MS 5 // Debouncing delay for umbilical (ms)

#define LOG_CAN_ID SEQ_PYRO_LOG_CAN_ID // ID of log messages
#define LOG_PERIOD 1000 // Period of log messages

#define ADC_PERIOD_MS 10 // Period of inductive sensor reading

#define LED_BLINK_PERIOD_GROUND_MS 1000
#define LED_BLINK_PERIOD_FLYING_MS 250

#endif	/* CONFIG_H */

