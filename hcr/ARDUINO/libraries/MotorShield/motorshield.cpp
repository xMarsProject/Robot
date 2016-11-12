/*####################################################################
 FILE: motorshield.cpp
 VERSION: 1.0
 PURPOSE: Dual DC Motor Shield library for Arduino

 HISTORY:
 Mirko Prosseda - Original version (23/06/2013)
#######################################################################*/


#include "motorshield.h"
#include "Wire.h"


void motorshield::initialize()
{
    // set digital pins direction
    pinMode(In1, OUTPUT);
    pinMode(In2, OUTPUT);
    pinMode(EnA, OUTPUT);
    pinMode(In3, OUTPUT);
    pinMode(In4, OUTPUT);
    pinMode(EnB, OUTPUT);
    // reset digital pins and disables both channels
    digitalWrite(In1, LOW);
    digitalWrite(In2, LOW);
    digitalWrite(In3, LOW);
    digitalWrite(In4, LOW);
    digitalWrite(EnA, LOW);
    digitalWrite(EnB, LOW);

    Wire.begin();       // initialize I2C port
    // set temperature reading resolution
    Wire.beginTransmission(TCN75Addr); // address the sensor
    Wire.write(ConfigReg);             // point at Configuration Register
    Wire.write(0x00);                  // set resolution at 0.5°C
    Wire.endTransmission();            // execute command
    // select Temperature Register for next readings
    Wire.beginTransmission(TCN75Addr); // address the sensor
    Wire.write(TempReg);               // point at Temperature Register
    Wire.endTransmission();            // execute command
}

// Set direction for a specific motor channel
void motorshield::setMotorDir(uint8_t ch, uint8_t dir)
{
    if((dir == 0) || (dir == 1)) // dir parameter must be 0 or 1
    {
        switch(ch)               // ch parameter must be 1 or 2
        {
            case 1:
                digitalWrite(In1, dir);
                digitalWrite(In2, !dir);
                break;
            case 2:
                digitalWrite(In3, dir);
                digitalWrite(In4, !dir);
                break;
        }
    }
}

// Set speed percentage for a specific motor channel
void motorshield::setMotorSpeed(uint8_t ch, uint8_t speed)
{
    if(speed <= 100)     // speed parameter must be from 0% to 100%
    {
        switch(ch)       // ch parameter must be 1 or 2
        {
            case 1:
                analogWrite(EnA, (float)speed * 2.56);
                break;
            case 2:
                analogWrite(EnB, (float)speed * 2.56);
                break;
        }
    }
}

// Read current absorption on a specific channel
float motorshield::getCurrent(uint8_t ch)
{
    // when you read motor current onsider that it is powered in pwm mode,
    // so also current readings will be pulsed and you need to filter tham
    // via a digital low pass filter
    if(ch == 1) return (((float)analogRead(SenA) * 0.002034505));      // = (1 / 1024 * 5) / 2.4;
    else if(ch == 2) return (((float)analogRead(SenB) * 0.002034505)); // = (1 / 1024 * 5) / 2.4;
    return -1; // error: ch value must be 1 or 2
}

// Read current absorption on a specific channel
float motorshield::getTemperature()
{
    signed int tempreg;
    float temperature;

    // read data from Temperature Register
    Wire.requestFrom(TCN75Addr, 2); // request 2 bytes from the sensor
    tempreg = Wire.read();          // first byte of the Temperature Register
    tempreg = tempreg << 8;         // 8-steps left shift
    tempreg |= Wire.read();         // second byte of the Temperature Register
    tempreg = tempreg >> 4;         // 4-steps right shift

    // evaluate ambient temperature
    temperature = (float)tempreg / 16; // Temperature Register value is divided by 16

    return temperature;
}
