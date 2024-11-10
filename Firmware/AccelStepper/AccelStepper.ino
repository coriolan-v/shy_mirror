#include <AccelStepper.h>

#define stepPin MISO
#define directionPin MOSI
#define enPin 5
#define HallSensor SCK

// 0-360 of the big cog
int calculatedPosition = 0;
int calculatedNumberOfRots = 0;

// Normally 0-3200 of the small cog
// 0-25600 of the large cog
int stepperPositionLib = 0;

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin);

#define microstep 32 // 1 turn of big cog = 32000

void setup()
{  
  pinMode(HallSensor, INPUT_PULLUP);

  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);
  // Change these to suit your stepper if you want
  



 stepper.setMaxSpeed(6000);
  stepper.setAcceleration(2000);
  stepper.moveTo(32000); // 32000 = 1 turn

  //homing();
}

void testMotor()
{
 

  stepper.run();
}

void loop()
{
  testMotor();
    // If at the end of travel go to the other end
    // if (stepper.distanceToGo() == 0)
    //   stepper.moveTo(-stepper.currentPosition());

     
}

void homing()
{
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(3000);
  stepper.setAcceleration(2000);
  stepper.moveTo(3200 * 5);

  while(digitalRead(HallSensor) == HIGH){
    stepper.run();
    Serial.println(stepper.currentPosition());
  }

  stepper.setCurrentPosition(0);
  stepperPositionLib = 0;
  Serial.println("Homing Complete");

  
}


