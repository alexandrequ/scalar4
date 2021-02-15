#include "barometre.h"
#include "i2c.h"
#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include "timing.h"

void init_pressure(uint8_t prom[SIZE_PROM]){   
    uint8_t commande[1]={PROM_BEGIN};
    write_I2C1(SLAVE,commande,1);
    read_I2C1(SLAVE,prom,SIZE_PROM);
    
}

void get_measure(uint8_t prom[SIZE_PROM],long resultat[2]){
    short ac1=(prom[AC1]<<8)+prom[AC1+1];
    short ac2=(prom[AC2]<<8)+prom[AC2+1];
    short ac3=(prom[AC3]<<8)+prom[AC3+1];
    unsigned short ac4=(prom[AC4]<<8)+prom[AC4+1];
    unsigned short ac5=(prom[AC5]<<8)+prom[AC5+1];
    unsigned short ac6=(prom[AC6]<<8)+prom[AC6+1];
    short b1=(prom[B1]<<8)+prom[B1+1];
    short b2=(prom[B2]<<8)+prom[B2+1];
    //short mb=(prom[MB]<<8)+prom[MB+1];
    short mc=(prom[MC]<<8)+prom[MC+1];
    short md=(prom[MD]<<8)+prom[MD+1];
    
    
    
    uint8_t commande[2]={CONTROL_REGISTER,CONVERT_TEMPERATURE};
    write_I2C1(SLAVE,commande,2);
    sleep(CONV_TIME_T);
    uint8_t read_adc[1]={READ_ADC};
    write_I2C1(SLAVE,read_adc,1);
    uint8_t UT[2]={0};
    read_I2C1(SLAVE,UT,2);
    _LATB15=0;
    long ut=(UT[0]<<8)+UT[1];
    commande[0]=CONTROL_REGISTER;
    commande[1]=CONVERT_PRESSURE;
    write_I2C1(SLAVE,commande,2);
    sleep(CONV_TIME_P);
    write_I2C1(SLAVE,read_adc,1);
    uint8_t UP[2];
    read_I2C1(SLAVE,UP,2);
    long up=(UP[0]<<8)+UP[1];
    
    long X1=(ut-ac6)*ac5/16384;
    long X2=mc*1024/(X1+md);
    long B5=X1+X2;
    long T=(B5+8)/16;
    
    long B6=B5-4000;
    X1=(b2*(B6*B6/2048))/1024;
    X2=ac2*B6/1024;
    long X3=X1+X2;
    long B3=(ac1*4+X3+2)/4;
    X1=ac3*B6/4096;
    X2=(b1*(B6*B6/2048))/32768;
    X3=((X1+X2)+2)/4;
    unsigned long B4=ac4*(unsigned long)(X3+32768)/16384;
    unsigned long B7=((unsigned long)up -B3)*5000;
    long p;
    if(B7<0x80000000){
        p=(B7*2)/B4;
    }
    else{
        p=(B7/B4)*2;
    }
    X1=(p/256)*(p/256);
    X1=(X1*3038)/32768;
    X2=(-7357*p)/32768;
    p=p+(X1+X2+3791)/16;

    resultat[0]= p;
    resultat[1]=T;
}

