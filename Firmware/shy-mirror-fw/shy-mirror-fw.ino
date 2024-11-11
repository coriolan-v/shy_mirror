// M4 express  itsy Bitsy
#include "Funcs.h"

void setup() {
  Serial.begin(115200);
  
  initMotor();

  //motorHoming();

  initSensors();

  //testZones();
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensorsDelay();

  detectPeopleZones();
 
  runMotor();

  lonelyAnimations();

  calibration();

 // readSerial();
}



const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

void readSerial() {
    recvWithEndMarker();
    showNewData();
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

int receivedPos = 0;

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        receivedPos = atoi(receivedChars);
        Serial.println(receivedPos);

        moveto(computeShortestDistance(receivedPos));
        newData = false;
    }
}