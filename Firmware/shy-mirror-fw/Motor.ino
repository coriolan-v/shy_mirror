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

int motorPosZonesPlus[numberOfSensors] = { -3000, 1500, 5500, 9500, 13500, 17500, 21500, 25500 };
int motorPosZonesMinus[numberOfSensors] = { -3000, -7000, -11000, -15000, -19000, -23000, -27000, -31000 };

int homePosition = 1500;

unsigned long stampMill_return;

// Define a stepper and the pins it will use
//AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin);

unsigned long prevMills_printMotorPos = 0;

void initMotor() {
  pinMode(HallSensor, INPUT_PULLUP);

  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);

  stepper.setMaxSpeed(8000);
  stepper.setAcceleration(5000);

  stepper.setCurrentPosition(0);
  // stepper.moveTo(32000);
}


void testZones() {
  for (int i = 0; i < numberOfSensors; i++) {
    Serial.print("Zone: ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(motorPosZones[i]);
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

  Serial.println("start HOMING");

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

  stepper.setMaxSpeed(6000);
  stepper.setAcceleration(4000);
}

void moveto(int newPos) {
  //stepper.setMaxSpeed(6000);
  //stepper.setAcceleration(2000);
  stepper.moveTo(newPos);
}

void move(int newPos) {
  stepper.move(newPos);
}

long diffPos = 0;
long newTargetPos = 0;
int computeShortestDistance(long targetPos) {
  diffPos = targetPos - stepper.currentPosition();

  if (diffPos > 16000) {
    newTargetPos = diffPos - 32000;
  } else if (diffPos <= -16000) {
    newTargetPos = diffPos + 32000;
  } else {
    newTargetPos = diffPos;
  }

  Serial.print("Current pos: ");
  Serial.print(stepper.currentPosition());
  Serial.print(", Target pos: ");
  Serial.print(targetPos);
  Serial.print(", diffPos: ");
  Serial.print(diffPos);
  Serial.print(", newTargetPos: ");
  Serial.println(newTargetPos);

  return newTargetPos;
}

unsigned long prevMill_animation = 0;

int randomPos[10] = {-8000, -4000, -3000, -2000, 0, 1000, 2000, 3000, 4000, 6000};

bool personDetected = false;

void lonelyAnimations() {

  if (personDetected == false) {
    if (millis() - stampMill_return > 10000 && millis() - stampMill_return < 11000) {
      // stampMill_return = millis();
      stepper.moveTo(homePosition);

      Serial.println("NO ONE, MOVING TO HOME POS");
    }

    if (millis() - stampMill_return > 11000 && millis() - prevMill_animation > 30000) {
       prevMill_animation = millis();

      int newRandomPos = randomPos[random(0, 10)];

      stepper.moveTo(newRandomPos);

      Serial.print("NO ONE, MOVING TO RANDOM POS "); Serial.println(newRandomPos);
    }
  }
}


unsigned long prevMill_calibration = 0;
unsigned long calibrationTimeMinute = 30;
void calibration()
{
  if(millis() - prevMill_calibration >= (calibrationTimeMinute * 60000)){
      prevMill_calibration = millis(); 

      Serial.println("HOMING TIME");

      motorHoming();
  }
}




void test() {
  if (millis() > 5000 && millis() < 10000) {
    stepper.moveTo(4888);
  }

  if (millis() > 10000) {
    stepper.moveTo(16000);
  }
}