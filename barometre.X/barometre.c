/*
 * File:   barometre.c

 * Author: alexis
 *
 * Created on 2 avril 2019, 11:32
 */
#include "barometre.h"

void init_pressure(uint16_t prom[SIZE_PROM/2]){

    uint8_t commande[1]={PROM_BEGIN};
    write_I2C1(SLAVE,commande,1);
    uint8_t data[SIZE_PROM]={0};
    read_I2C1(SLAVE,data,SIZE_PROM);

    int i;
    for(i=0;i<SIZE_PROM/2;i++){
        prom[i]=data[2*i]*256+data[2*i+1];
    }

}

void get_measure(uint16_t prom[SIZE_PROM/2],double resultat[2]){
    uint8_t commande[1]={CONVERT_PRESSURE};
    write_I2C1(SLAVE,commande,1);
    sleep(CONV_TIME);
    uint8_t D1[3];
    read_I2C1(SLAVE,D1,3);

    commande[0]=CONVERT_TEMPERATURE;
    write_I2C1(SLAVE,commande,1);
    sleep(CONV_TIME);
    uint8_t D2[3];
    read_I2C1(SLAVE,D2,3);

    long d1=D1[0]*65536+D1[1]*256+D1[2];
    long d2=D2[0]*65536+D2[1]*256+D2[2];
    long dT=d2-prom[TREF]*256;
    double temp=2000.0+(dT+0.0)*(prom[TEMPSENS]+0.0)/(8388608.0);
    double off=prom[OFF]*131072.0+(prom[TCO]+0.0)*(dT+0.0)/64;
    double sens=(prom[SENS]+0.0)*65536+(prom[TCS]+0.0)*(dT+0.0)/128.0;
    double pressure=((d1+0.0)*sens/2097152.0-off)/32768.0;

    resultat[0]= pressure;
    resultat[1]=temp;
}


