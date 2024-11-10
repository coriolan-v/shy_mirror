#include <Wire.h>
#include <VL53L1X.h>
//#include "Funcs.h"

bool verboseSensor = true;

// The number of sensors in system.
const uint8_t sensorCount = 8;

#define commonDistance 150

// Distance in mm
int lowerTreshold[sensorCount] = { commonDistance, commonDistance, commonDistance, commonDistance, commonDistance, commonDistance, commonDistance, commonDistance };
int upperTreshld[sensorCount] = { 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000 };

const uint8_t numReadings = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { A0, A1, A2, A3, 9, 10, 11, 12 };

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
bool startReadSensors = false;
void readSensorsDelay() {

  if (millis() - prevMill_sensorFPS >= readSensorSampling) {
    prevMill_sensorFPS = millis();

   // stampStartRead = millis();
    sensorReadOrder = 0;
    startReadSensors = true;

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



  if (startReadSensors == true) {

    if (millis() - prevMill_sensorDelay >= sensorDelay) {
      prevMill_sensorDelay = millis();

      sensorReadRaw[sensorReadOrder] = sensors[sensorReadOrder].read(false);

      sensorReadMean[sensorReadOrder] += (sensorReadRaw[sensorReadOrder] - sensorReadMean[sensorReadOrder]) / numReadings;

      sensorReadOrder++;

      if (sensorReadOrder == 8) {
        startReadSensors = false;
      }
    }
  }
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

void detectPeopleZones() {

  if (millis() - prevMill_sensorProcess >= readSensorProcess) {
    prevMill_sensorProcess = millis();

    for (uint8_t i = 0; i < sensorCount; i++) {
      if (sensorReadMean[i] < lowerTreshold[i] && sensorReadMean[i] > 2) {

        if (verboseSensor == true) {
          printTime();
          Serial.print("Person detected zone ");
          Serial.print(i);

          Serial.print(", moving to: ");
          Serial.println(motorPosZones[i]);
        }


        moveto(motorPosZones[i]);
      }
    }
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
