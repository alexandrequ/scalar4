#include "MPU9250.h"

MPU9250 IMU(Wire, 0x68);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  int status=IMU.begin();
  if(status<0){
    Serial.println("initialisation ratee");
    while(1);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
    IMU.readSensor();
    // display the data
    Serial.println(IMU.getAccelX_mss(),6);
    
    Serial.println(IMU.getAccelY_mss(),6);
    
    Serial.println(IMU.getAccelZ_mss()+9.3,6);
    delay(100);
}
