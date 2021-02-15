#include <SFE_BMP180.h>

void actualise_measure(SFE_BMP180& baro, double& P,double& T){
  char measure_time=baro.startTemperature();
  delay(measure_time);
  if(baro.getTemperature(T)==0){
    Serial.println("erreur sur la mesure de la temperature");
  }
  measure_time=baro.startPressure(0);
  delay(measure_time);
  if(baro.getPressure(P,T)==0){
    Serial.println("erreur sur la mesure de la pression");
  }
  
}

void actualise_altitude(const double& P, const double& P0, double& H){
   H=44330*(1-pow(P/P0,1/5.255));
}

void setup() {
  Serial.begin(9600);
  SFE_BMP180 baro;
  double p=0.0;
  double& P=p;
  double t=0.0;
  double& T=t;
  double h=0.0;
  double& H=h;
  double p0=0.0;
  double& P0=p0;
  
  if(baro.begin()==0){
    Serial.println("erreur d'initialisation du barometre");
  }
  actualise_measure(baro, P0, T);
  while(true){
    actualise_measure(baro, P, T);
    actualise_altitude(P,P0,H);
    Serial.print("pression:");
    Serial.println(P);
    Serial.print("temperature:");
    Serial.println(T);
    Serial.print("altitude:");
    Serial.println(H);

    delay(1000);
  }
  
}

void loop() {
  

}
