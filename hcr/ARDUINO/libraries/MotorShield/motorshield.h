/*####################################################################
 FILE: motorshield.h
 VERSION: 1.0
 PURPOSE: Dual DC Motor Shield library for Arduino

 HISTORY:
 Mirko Prosseda - Original version (23/06/2013)
#######################################################################*/


#ifndef motorshield_h
#define motorshield_h

#include "Arduino.h"

#define In1 9
#define In2 10
#define EnA 3
#define In3 2
#define In4 6
#define EnB 5
#define SenA A0
#define SenB A1

#define TCN75Addr B1001000 // temperature sensor address
#define ConfigReg 0x01     // Configuration Register
#define TempReg 0x00       // Temperature Register


class motorshield
{
public:
    void initialize();                             // initialize the Motor Shield hardware
    void setMotorDir(uint8_t ch, uint8_t dir);     // set direction for a specific motor channel
    void setMotorSpeed(uint8_t ch, uint8_t speed); // set speed percentage for a specific motor channel
    float getCurrent(uint8_t ch);                  // read current absorption on a specific channel
    float getTemperature();                        // read board temperature
};
#endif
