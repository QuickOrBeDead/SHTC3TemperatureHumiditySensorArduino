#include "TemHumSensor.h"

bool SHTC3TemHumSensor::connectToI2C(TwoWire &wirePort) {
  Wire.begin(); 
  Wire.beginTransmission(SHTC3_I2C_ADDR);
  if(!Wire.endTransmission()) {
    return true;
  } 
  return false;     
}

uint16_t SHTC3TemHumSensor::getReadID(void) {
  uint8_t buffer[2];
 
  readCommand(SHTC3_CMD_READ_ID, &buffer[0], 2);
  return (buffer[0] << 8) | buffer[1];
}

void SHTC3TemHumSensor::sleep(void) {
  sendCommand(SHTC3_CMD_SLEEP); 
}

void SHTC3TemHumSensor::wakeup(void) {
  sendCommand(SHTC3_CMD_WAKE_UP); 
}

void SHTC3TemHumSensor::reset(void) {
  sendCommand(SHTC3_CMD_SOFTWARE_RESET); 
}

uint8_t SHTC3TemHumSensor::getTemValue(float *value) {
  float rawValue;
  
  uint8_t error = readFloat(SHTC3_CMD_CSE_TF_NPM, &rawValue);
  if (error != 0) {
    return error;
  }

  *value = ((rawValue / 65536) * 175) - 45.0f;

  return error;
}

uint8_t SHTC3TemHumSensor::getHumValue(float *value) {
  float rawValue;
  
  uint8_t error = readFloat(SHTC3_CMD_CSE_RHF_NPM, &rawValue);
  if (error != 0) {
    return error;
  }

  *value = (rawValue / 65536) * 100;

  return error;
}

uint8_t SHTC3TemHumSensor::readFloat(uint16_t cmd, float *value) {
  uint8_t buffer[3];

  uint8_t error = readCommand(cmd, &buffer[0], 2);
  if (error != 0) {
    return error;
  }

  *value = (float)((buffer[0] << 8) | buffer[1]);

  return error;
}

void SHTC3TemHumSensor::sendCommand(uint16_t cmd) {
  Wire.beginTransmission(SHTC3_I2C_ADDR);
  Wire.write((((uint16_t)cmd) >> 8));
  Wire.write((((uint16_t)cmd) & 0x00FF));
  Wire.endTransmission();
}

uint8_t SHTC3TemHumSensor::readCommand(uint16_t cmd, uint8_t *pdata, uint8_t size) {
  Wire.beginTransmission(SHTC3_I2C_ADDR); 
  Wire.write((((uint16_t)cmd) >> 8));
  Wire.write((((uint16_t)cmd) & 0x00FF));
  uint8_t endTransmissionResult = Wire.endTransmission();

  if (endTransmissionResult != 0) {
    return 1;
  }
  
  size_t sizeToRead = size + 1; // include checksum  
  size_t read = Wire.requestFrom(SHTC3_I2C_ADDR, sizeToRead);
  if (read != sizeToRead) {
    return 1;
  }

  for (uint8_t i = 0; i < sizeToRead; i++){
    *(pdata+i) = Wire.read();
  }

  return checkCrc(pdata, size);
}


uint8_t SHTC3TemHumSensor::checkCrc(uint8_t *pdata, uint8_t size){
  uint8_t bit;        // bit mask
  uint8_t crc = 0xFF; // calculated checksum
  uint8_t byteCtr;    // byte counter
  uint8_t checksum = *(pdata+size);

  // calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < size; byteCtr++) {
    crc ^= (*(pdata+byteCtr));
    for (bit = 8; bit > 0; --bit) {
      if(crc & 0x80) {
        crc = (crc << 1) ^ CRC_POLYNOMIAL;
      } else {
        crc = (crc << 1);
      }
    }
  }

  // verify checksum
  if (crc == checksum) {
    return 0;
  }

  return 2;
}