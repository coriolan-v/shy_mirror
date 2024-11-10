// M4 express  itsy Bitsy
#include "Funcs.h"

void setup() {
  Serial.begin(115200);
  
  initMotor();

  //motorHoming();

  initSensors();
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensorsDelay();

  //detectPeopleZones();
 

  runMotor();
}
