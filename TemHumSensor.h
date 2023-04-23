
#ifndef __Deneyap_TemHumSensor_H
#define __Deneyap_TemHumSensor_H

#include <Wire.h>
#include <Arduino.h>

// https://www.mouser.com/datasheet/2/682/Sensirion_04202018_HT_DS_SHTC3_Preliminiary_D2-1323493.pdf
/*
  - I2C Address
    The I2C device address is:
      | SHTC3        | Hex. Code | Bin. Code |
      | I2C address  | 0x70      | 111’0000  |

    Each transmission sequence begins with START condition (S) and ends with an (optional) STOP condition (P) as described in the I2C-bus specification.

  - Power-Up, Sleep, Wakeup
    Upon VDD reaching the power-up voltage level VPOR, the SHTC3 enters idle state after a duration of tPU. After that, the sensor has to be set to 
    sleep mode with the command:
      | Command | Hex. Code | Bin. Code           |
      | Sleep   | 0xB098    | 1011’0000’1001’1000 |
   
    When the sensor is in sleep mode, it requires the following wake up command before any further communication:
      | Command | Hex. Code | Bin. Code           |
      | Wakeup  | 0x3517    | 0011’0101’0001’0111 |

  - Measuring and Reading the Signals
    Each measurement cycle contains a set of four commands, each initiated by the I2C START condition and ended by the I2C STOP condition:
      1. Wakeup command
      2. Measurement command
      3. Read out command
      4. Sleep command

  - Read-out of ID Register
    The SHTC3 has an ID register which contains an SHTC3-specific product code. The read-out of the ID register can be used to verify 
    the presence of the sensor and proper communication. The command to read the ID register is;

    | Command           | Hex. Code | Bin. Code           |
    | Read ID register  | 0xEFC8    | 1110’1111’1100’1000 |

*/
//The default I2C address for the SHTC3 is 0x70
#define SHTC3_I2C_ADDR                             0x70
#define SHTC3_CMD_READ_ID                          0xEFC8
#define SHTC3_CMD_WAKE_UP                          0x3517
#define SHTC3_CMD_SLEEP                            0xB098
#define SHTC3_CMD_SOFTWARE_RESET                   0x805D

#define SHTC3_CMD_CSE_TF_NPM                       0x7CA2  /* Clock stretching enabled, read T first, Normal power mode */
#define SHTC3_CMD_CSE_RHF_NPM                      0x5C24  /* Clock stretching enabled, read RH first, Normal power mode */

#define SHTC3_PRODUCT_CODE                         0x0807
#define SHTC3_PRODUCT_CODE_MASK                    0x083F

#define CRC_POLYNOMIAL                             0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

class SHTC3TemHumSensor { 
  public:
    bool connectToI2C(TwoWire &wirePort = Wire); 
    uint8_t getTemValue(float *value);
    uint8_t getHumValue(float *value);
    void sleep(void);
    void wakeup(void);    
    void softwareReset(void);                            
  
  private:
    uint16_t getReadID(void);
    uint16_t getProductCode(void);
    uint8_t validateProductCode(void);
    uint8_t readFloat(uint16_t cmd, float *value);
    void sendCommand(uint16_t cmd);   
    uint8_t readCommand(uint16_t cmd, uint8_t *pdata, uint8_t size);
    uint8_t checkCrc(uint8_t *pdata, uint8_t size);
};

#endif // End of __Deneyap_TemHumSensor_H definition check