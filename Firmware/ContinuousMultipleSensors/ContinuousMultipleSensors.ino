/*
This example shows how to set up and read multiple VL53L1X sensors connected to
the same I2C bus. Each sensor needs to have its XSHUT pin connected to a
different Arduino pin, and you should change sensorCount and the xshutPins array
below to match your setup.

For more information, see ST's application note AN4846 ("Using multiple VL53L0X
in a single design"). The principles described there apply to the VL53L1X as
well.
*/

#include <Wire.h>
#include <VL53L1X.h>

// The number of sensors in your system.
const uint8_t sensorCount = 8;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 12, 11, 10, 9, A3, A2, A1, A0 };

//{9, 10, 11, 12 };
//{A0, A1, A2, A3 };

VL53L1X sensors[sensorCount];

 enum DistanceMode { Short, Medium, Long, Unknown };

void setup()
{
  while (!Serial) {}
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    sensors[i].setTimeout(500);
    if (!sensors[i].init())
    {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1);
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);

    sensors[i].setDistanceMode(VL53L1X::Medium);
  sensors[i].setMeasurementTimingBudget(500);

    sensors[i].startContinuous(100);

    //sensors[i].setDistanceMode(Short);
  }
}

void loop()
{
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    //Serial.print(sensors[i].read());

    sensors[i].read();//

     Serial.print("range: ");
  Serial.print(sensors[i].ranging_data.range_mm);
  Serial.print("\tstatus: ");
  Serial.print(VL53L1X::rangeStatusToString(sensors[i].ranging_data.range_status));
  if(VL53L1X::rangeStatusToString(sensors[i].ranging_data.range_status) == "range valid") {
    Serial.println("YAY");
  }
  Serial.print("\tpeak signal: ");
  Serial.print(sensors[i].ranging_data.peak_signal_count_rate_MCPS);
  Serial.print("\tambient: ");
  Serial.print(sensors[i].ranging_data.ambient_count_rate_MCPS);
    //Serial.print(sensors[i].distance());
    delay(10);
    //if (sensors[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.print(" ");
  }
  Serial.println();
}
