#include "xsens_config.h"
#include "xbus.h"

uint16_t acknowledge_timeout = 250;

void setAcknowledgeTimeout(uint16_t timeout){
    acknowledge_timeout = timeout;
}

/*
bool restartMTi(bool startInConfigState){
    if (!receive_xbus_buf(2000) || xbus_receive_buf->MID != 0x3E) // WakeUp              
                return false;
    
    if(startInConfigState){
        xbus_send_buf->MID = 0x3F; // WakeUpAck
        xbus_send_buf->dataSize = 0;
        send_xbus_buf();
        
        // Warning: there is no way to check if the XSens is actually in config
        // state this way.
    }
    return true;
}*/

bool configOptions(XSensOption options[], int nb){
    
    uint8_t flags = 0x00;
    int i=0;
    for(i=0; i<nb; i++)
        flags += options[i];

    // Configuration of the Xsens option (optional)
    xbus_send_buf->MID = 0x48; // SetOptionFlags
    xbus_send_buf->data[0] = 0x00; // Set flags
    xbus_send_buf->data[1] = 0x00;
    xbus_send_buf->data[2] = 0x00;
    xbus_send_buf->data[3] = flags;
    xbus_send_buf->data[4] = 0x00; // Clear flags
    xbus_send_buf->data[5] = 0x00;
    xbus_send_buf->data[6] = 0x00;
    xbus_send_buf->data[7] = 0x9F - flags;
    xbus_send_buf->dataSize = 8;
    send_xbus_buf();

    // Checking acknowledgment
    return (receive_xbus_buf(acknowledge_timeout) && xbus_receive_buf->MID == 0x49); // SetOptionFlagsAck

}

bool configOutputs(uint8_t frequency, XSensOutput measures[], int nb){
    int i = 0;
    
    xbus_send_buf->MID = 0xC0; // SetOutputConfiguration   
    for(i=0; i<nb; i++){
        xbus_send_buf->data[4*i]   = (uint8_t)(measures[i]>>8); //Output ID part 1
        xbus_send_buf->data[4*i+1] = (uint8_t)measures[i]; //Output ID part 2
        xbus_send_buf->data[4*i+2] = 0x00; // Nothing
        xbus_send_buf->data[4*i+3] = frequency;
    } 
    xbus_send_buf->dataSize = 4*nb;
    
    send_xbus_buf();
    
    // Checking acknowledgment
    return(receive_xbus_buf(acknowledge_timeout) && xbus_receive_buf->MID == 0xC1); // SetOutputConfigurationAck
}

bool goToConfig(){
    xbus_send_buf->MID = 0x30; // GoToConfig
    xbus_send_buf->dataSize = 0;
    send_xbus_buf();
    
    // Checking acknowledgment
    return (receive_xbus_buf(acknowledge_timeout) && xbus_receive_buf->MID == 0x31);
}

bool goToMeasure(){
    xbus_send_buf->MID = 0x10; // GoToMeasurement
    xbus_send_buf->dataSize = 0;
    send_xbus_buf();
    
    // Checking acknowledgment
    return (receive_xbus_buf(acknowledge_timeout) && xbus_receive_buf->MID == 0x11);
}

bool setNoRotation(uint16_t timeout){
    xbus_send_buf->MID = 0x22;
    xbus_send_buf->data[0] = (uint8_t)(timeout>>8);
    xbus_send_buf->data[1] = (uint8_t)(timeout);
    xbus_send_buf->dataSize = 2;
    send_xbus_buf();
    
    // Checking acknowledgment
    return (receive_xbus_buf(acknowledge_timeout) && xbus_receive_buf->MID == 0x23);
}