// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define a stepper and the pins it will use
#define stepPin MISO
#define directionPin MOSI
#define enPin 5
AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin);

void setup()
{  
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);
  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(6000);
  stepper.setAcceleration(2000);
  stepper.moveTo(32000);
}

void loop()
{
    // If at the end of travel go to the other end
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(-stepper.currentPosition());

    stepper.run();
}
