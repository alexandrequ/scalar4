#include "xbus.h"
#include "data_processing.h"
#include <xc.h>
#include "trajecto.h"

float xbus_buf_to_float(int i){
    uint8_t data_raw[4];
    data_raw[3]=xbus_receive_buf->data[i];
    i++;
    data_raw[2]=xbus_receive_buf->data[i];
    i++;
    data_raw[1]=xbus_receive_buf->data[i];
    i++;
    data_raw[0]=xbus_receive_buf->data[i];
    return (*((float*)data_raw));
}

uint32_t xbus_buf_to_uint32(int i){
    uint8_t data_raw[4];
    data_raw[3]=xbus_receive_buf->data[i];
    i++;
    data_raw[2]=xbus_receive_buf->data[i];
    i++;
    data_raw[1]=xbus_receive_buf->data[i];
    i++;
    data_raw[0]=xbus_receive_buf->data[i];
    return (*((uint32_t*)data_raw));
}

void read_xbus_data(){

    if (xbus_receive_buf->MID != 0x36){  // On verifie qu'il s'agit du bon message
        return;
    }

    uint16_t i = 0;
    unsigned int j = 0;
    uint8_t data_id_raw[2];
    uint16_t data_id;
    uint8_t data_len;

    //acquisition_data.date=timestamp();
        // TODO c'est quoi la taille d'un long double ?


    while (i < xbus_receive_buf->dataSize){


        data_id_raw[1]=xbus_receive_buf->data[i];
        i++;
        data_id_raw[0]=xbus_receive_buf->data[i];
        i++;

        //data_id = *((uint16_t*)data_id_raw); // segfault here
        data_id = (uint16_t)data_id_raw[1]*16*16+(uint16_t)data_id_raw[0];

        data_len=xbus_receive_buf->data[i];
        i++;

        switch (data_id){
            case 0x1060: // timestamp
                acquisition_data.sample_10kHz_date = xbus_buf_to_uint32(i);
                i+=4;
                break;

            case 0x2010: // quaternion
                for(j = 0; j<4 ; j++){
                    acquisition_data.quat[j] = xbus_buf_to_float(i);

                    i=i+4;
                }
                break;

            case 0x4010: // deltaV
                for(j = 0; j<3 ; j++){

                    acquisition_data.deltaV[j] = xbus_buf_to_float(i);

                    i=i+4;
                }
                break;

            case 0x4020: // acceleration
                for(j = 0; j<3 ; j++){

                    acquisition_data.acc[j] = xbus_buf_to_float(i);

                    i=i+4;
                }
                break;

            case 0x8020: // angular velocity
                for(j = 0; j<3 ; j++){
                    acquisition_data.gyr[j] = xbus_buf_to_float(i);

                    i=i+4;
                }
                break;

            case 0x8030: // deltaQ
                for(j = 0; j<4 ; j++){
                    acquisition_data.deltaQ[j] = xbus_buf_to_float(i);

                    i=i+4;
                }
                break;

            case 0xC020: // magnetic field
                for(j = 0; j<3 ; j++){
                    acquisition_data.magn[j] = xbus_buf_to_float(i);

                    i=i+4;
                }
                break;

        }
    }
}
