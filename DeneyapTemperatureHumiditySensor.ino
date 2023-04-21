#include "TemHumSensor.h"

SHTC3TemHumSensor TemHum;  

void setup() {
  Serial.begin(115200);

  while (!TemHum.connectToI2C()) {        
    delay(3000);
    Serial.println("Unable to connect to I2C");
  }

  TemHum.wakeup();
  TemHum.reset();
  
  Serial.println("Connected to I2C");

  uint8_t sensorId = TemHum.getReadID();
  Serial.printf("sensorId = %d\n", sensorId);
}

void loop() {
  float value;
  uint8_t error;
  
  error = TemHum.getTemValue(&value);
  if (error == 0) {
    Serial.printf("Temperature = %f\n", value);
  }

  error = TemHum.getHumValue(&value);
  if (error == 0) {
    Serial.printf("Humidity = %f\n", value);
  }

  TemHum.sleep();
  delay(120000);
  TemHum.wakeup();
}