#ifndef XC_HEADER_H
#define	XC_HEADER_H

#define SLAVE 0x76
#define RESET 0x1E
#define CONVERT_PRESSURE 0x44
#define CONVERT_TEMPERATURE 0x54 
#define READ_ADC 0x00
#define SENS 0
#define OFF 1
#define TCS 2
#define TCO 3
#define TREF 4
#define TEMPSENS 5
#define SIZE_PROM 12
#define PROM_BEGIN 0xA2

#define CONV_TIME 5


#include "communication_I2C.h"
#include "timing.h"

void init_pressure(uint16_t prom[SIZE_PROM/2]);
void get_pressure(long sens,long off,long tcs,long tco,long tref,long tempsens,double resultat[2]);


#endif	/* XC_HEADER_TEMPLATE_H */

