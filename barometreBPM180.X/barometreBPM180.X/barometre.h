/**
 * @file barometre.h
 *
 * @author Alexis Vandewalle
 *
 * @brief this file contains different function to use a barometer bpm180
 */

#ifndef XC_HEADER_H
#define	XC_HEADER_H

#define SLAVE (0xEE>>1)
#define RESET 0xE0
#define CONTROL_REGISTER 0xF4
#define CONVERT_PRESSURE 0x74
#define CONVERT_TEMPERATURE 0x2E 
#define READ_ADC 0xF6
#define AC1 0
#define AC2 2
#define AC3 4
#define AC4 6
#define AC5 8
#define AC6 10
#define B1 12
#define B2 14
#define MB 16
#define MC 18
#define MD 20
#define SIZE_PROM 22
#define PROM_BEGIN 0xAA

#define CONV_TIME_P 8
#define CONV_TIME_T 5

#include <stdint.h>
/**
 * @brief a function to initialize the barometer bpm180, espacially to read the eprom of the sensor
 * 
 * @param prom an array of long whose size is the half of the size of the eprom in bytes. the value in this array will be set in the function
 */
void init_pressure(uint8_t prom[SIZE_PROM]);

/**
 * @brief a function to get the pressure and the temperature
 *
 * @param prom an array of long whose size is the half of the size of the eprom in bytes. It must contains the value of the prom of the sensor
 *
 * @param resultat an array of double which will contains the values of pressure and temperature in this order
 */
void get_measure(uint8_t prom[SIZE_PROM],long resultat[2]);


#endif

