#include <Wire.h>
#include <VL53L1X.h>
//#include "Funcs.h"

bool verboseSensor = true;

// The number of sensors in system.
const uint8_t sensorCount = 8;

#define commonDistance 1100

// Distance in mm
int lowerTreshold[sensorCount] = { commonDistance, commonDistance, commonDistance, 500, commonDistance, 500, commonDistance, 800 };
int upperTreshld[sensorCount] = { 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000 };

const uint8_t numReadings = 4;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 12, 11, 10, 9, A3, A2, A1, A0 };  //, A1, A2, A3, 9, 10, 11, 12 };

VL53L1X sensors[sensorCount];

int sensorReadRaw[sensorCount];
int sensorReadMean[sensorCount];

unsigned long prevMill_sensorFPS;
unsigned int readSensorSampling = 100;
unsigned long prevMill_sensorDelay;
unsigned int sensorDelay = 10;

unsigned long prevMill_sensorProcess;
unsigned int readSensorProcess = 25;


void initSensors() {
  //while (!Serial) {}
  //Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);  // use 400 kHz I2C

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++) {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    sensors[i].setTimeout(500);
    if (!sensors[i].init()) {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1)
        ;
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);

    sensors[i].startContinuous(50);
  }

  // for (uint8_t i = 0; i < sensorCount; i++) averageSensors[i].clear();
}

int sensorReadOrder = 0;
unsigned long stampStartRead = 0;
bool startReadSensors = true;
void readSensorsDelay() {

  if (millis() - prevMill_sensorFPS >= readSensorSampling) {
    prevMill_sensorFPS = millis();

    //  // stampStartRead = millis();
    //   sensorReadOrder = 0;
    //   startReadSensors = true;

    //float raw_temp = analogRead(tempPin[i]) / 9.31;


    if (verboseSensor == true) {

      printTime();
      //averageSensors
      for (int i = 0; i < sensorCount; i++) {
        Serial.print(sensorReadRaw[i]);
        Serial.print("/");
        Serial.print(sensorReadMean[i]);
        Serial.print("  ");
      }
      if (verboseSensor == true) Serial.println();
    }
    //Serial.print('\t');
  }

  // if (startReadSensors == true) {

  if (millis() - prevMill_sensorDelay >= sensorDelay) {
    prevMill_sensorDelay = millis();

    sensorReadRaw[sensorReadOrder] = sensors[sensorReadOrder].read(false);

    sensorReadMean[sensorReadOrder] += (sensorReadRaw[sensorReadOrder] - sensorReadMean[sensorReadOrder]) / numReadings;

    sensorReadOrder++;

    if (sensorReadOrder == 8) {
      sensorReadOrder = 0;
    }
  }
  // }
}

void readSensors() {

  if (millis() - prevMill_sensorFPS >= readSensorSampling) {
    prevMill_sensorFPS = millis();

    for (uint8_t i = 0; i < sensorCount; i++) {
      sensorReadRaw[i] = sensors[i].read(false);

      //if (sensors[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }

      //float raw_temp = analogRead(tempPin[i]) / 9.31;
      sensorReadMean[i] += (sensorReadRaw[i] - sensorReadMean[i]) / numReadings;

      if (verboseSensor == true) {
        //averageSensors
        Serial.print(sensorReadRaw[i]);
        Serial.print("/");
        Serial.print(sensorReadMean[i]);
        Serial.print("  ");
      }
      //Serial.print('\t');
    }
    if (verboseSensor == true) Serial.println();
  }
}

int old_targetZone = 0;

unsigned long stampMill_detect[sensorCount];

bool personDetectStamp[sensorCount];

void detectPeopleZones() {

  if (millis() - prevMill_sensorProcess >= readSensorProcess) {
    prevMill_sensorProcess = millis();

    for (uint8_t i = 0; i < sensorCount; i++) {
      if (sensorReadMean[i] < lowerTreshold[i] && sensorReadMean[i] > 2 && VL53L1X::rangeStatusToString(sensors[i].ranging_data.range_status) == "range valid") {

        int targetZone = 8;

        personDetected = true;

        //stampMill_detect[i] = millis();
        //personDetectStamp[i] = true;

        if (i == 0) targetZone = 4;
        if (i == 1) targetZone = 5;
        if (i == 2) targetZone = 6;
        if (i == 3) targetZone = 7;
        if (i == 4) targetZone = 0;
        if (i == 5) targetZone = 1;
        if (i == 6) targetZone = 2;
        if (i == 7) targetZone = 3;

        if (targetZone != old_targetZone) {

          old_targetZone = targetZone;

          stampMill_return = millis();

          if (millis() - stampMill_detect[i] > 2000) {

            moveto(motorPosZones[targetZone]);

            stampMill_detect[i] = millis();
            personDetectStamp[i] = false;
            if (verboseSensor) {
              printTime();
              Serial.print("Person detected zone ");
              Serial.print(i);
              Serial.print(", Current motor pos: ");
              Serial.print(stepper.currentPosition());
              Serial.print(", moving to: ");
              Serial.println(motorPosZonesPlus[targetZone]);
            }
          }

          //(motorPosZonesPlus[targetZone]);
        }


        // int randomDir = random(-1, 1);
        // if(randomDir == -1){
        //   move(4000);
        // } else {
        //   move(-4000);
        // }
      }
    }
  }

  if (millis() - stampMill_return > 10000) {
    personDetected = false;
  }
}

// if 1 person only: Move to zone + 1 or zone -1. Find the closest (wether it's CW or CCW)
void printTime() {
  unsigned long runMillis = millis();
  unsigned long allSeconds = millis() / 1000;
  int runHours = allSeconds / 3600;
  int secsRemaining = allSeconds % 3600;
  int runMinutes = secsRemaining / 60;
  int runSeconds = secsRemaining % 60;

  char buf[21];
  sprintf(buf, "%02d:%02d:%02d ", runHours, runMinutes, runSeconds);
  Serial.print(buf);
}
