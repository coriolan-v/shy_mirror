#include "Funcs.h"

bool verboseMotor = true;

#include <AccelStepper.h>

#define stepPin MISO
#define directionPin MOSI
#define enPin 5
#define HallSensor SCK

AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin);

// 0-360 of the big cog
int calculatedPosition = 0;
int calculatedNumberOfRots = 0;

// Normally 0-3200 of the small cog
// 0-25600 of the large cog
int stepperPositionLib = 0;

// The number of sensors in your system.
const uint8_t numberOfSensors = 8;

int motorPosZones[numberOfSensors] = { 0, 4000, 8000, 12000, 16000, 20000, 24000, 28000 };

// Define a stepper and the pins it will use
//AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin);

unsigned long prevMills_printMotorPos = 0;

void initMotor() {
  pinMode(HallSensor, INPUT_PULLUP);

  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);

  stepper.setMaxSpeed(8000);
  stepper.setAcceleration(5000);
  stepper.moveTo(32000);
}



void testZones() {
  for (int i = 0; i < numberOfSensors; i++) {
    Serial.print("Zone: ");
    Serial.println(i);
    stepper.runToNewPosition(motorPosZones[i]);
  }
}

void runMotor() {
  stepper.run();

  // stepperPositionLib = stepper.currentPosition() * 8;

  // if (verboseMotor == true) {
  //   if (millis() - prevMills_printMotorPos >= 100) {
  //     prevMills_printMotorPos = millis();

  //     Serial.print("Stepper pos: ");
  //     Serial.print(stepper.currentPosition());
  //     Serial.print(", lib pos: ");
  //     Serial.print(stepperPositionLib);
  //     Serial.println();
  //   }
  // }
}

void motorHoming() {
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(3000);
  stepper.setAcceleration(2000);
  stepper.moveTo(32000);

  while (digitalRead(HallSensor) == HIGH) {
    stepper.run();
    Serial.print(digitalRead(HallSensor));
    Serial.print("/");
    Serial.println(stepper.currentPosition());

    if (stepper.currentPosition() == 32000) break;
  }

  stepper.setCurrentPosition(0);
  stepperPositionLib = 0;
  Serial.println("Homing Complete");
}

void moveto(int newPos)
{
  //stepper.setMaxSpeed(6000);
  //stepper.setAcceleration(2000);
  stepper.moveTo(newPos);
}

void test(){
  if(millis() > 5000 && millis() < 10000){
     stepper.moveTo(4888);
  }

  if(millis() > 10000){
     stepper.moveTo(16000);
  }
}