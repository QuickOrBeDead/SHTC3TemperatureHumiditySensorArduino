#include "TemHumSensor.h"

SHTC3TemHumSensor TemHum;  

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(1000);
  }

  while (!TemHum.connectToI2C()) {        
    delay(3000);
    Serial.println("Unable to connect to I2C");
  }

  Serial.println("Connected to I2C");

  TemHum.wakeup();
  TemHum.softwareReset();

  delay(250);

  uint16_t productCode = TemHum.getProductCode();
  Serial.printf("Product code = 0x%03X\n", productCode);
}

void loop() {
  float value;
  uint8_t error;
  
  uint16_t productCode = TemHum.getProductCode();
  if (productCode != SHTC3_PRODUCT_CODE) {
    Serial.printf("Product code in SHTC3 ID register does not match. Product code should be 0x%03X but it is 0x%03X\n", SHTC3_PRODUCT_CODE, productCode);
    delay(2000);
    return;
  }

  error = TemHum.getTemValue(&value);
  if (error == 0) {
    Serial.printf("Temperature = %f\n", value);
  } else {
    Serial.printf("Read Temperature error: %d\n", error);
  }

  error = TemHum.getHumValue(&value);
  if (error == 0) {
    Serial.printf("Humidity = %f\n", value);
  } else {
    Serial.printf("Read Humidity error: %d\n", error);
  }

  TemHum.sleep();
  delay(60000);
  TemHum.wakeup();
}