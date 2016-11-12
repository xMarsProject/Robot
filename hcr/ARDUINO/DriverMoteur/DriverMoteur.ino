/*####################################################################
 FILE: motorshield_example.ino
 VERSION: 1.0
 PURPOSE: Dual DC Motor Shield test sketch

 Description:
 * Shield temperature are monitored on Serial Monitor
 * Motor speed and direction are controlled by a potentiometer.

 Connections:
 * this sketch needs two potentiometers attached on 3-pin strips labeled
 * "Analog Input"; they are used to change motor speed and direction

 HISTORY:
 Mirko Prosseda - Original version (23/06/2013)
#######################################################################*/


#include <motorshield.h>
 #include <Wire.h>

motorshield MotorShield;

const int analogPin1 = A2;
const int analogPin2 = A3;

int analogValue1, analogValue2;
uint8_t pwmValue1, pwmValue2;

float temperature;

void setup()
{
    MotorShield.initialize(); // initialize the Motor Shield hardware
    Serial.begin(9600);       // initialize Serial Port
    Serial.println("Dual DC Motor Shield test sketch");
}

void loop()
{
  // read both potentiometers values
  analogValue1 = analogRead(analogPin1);
  analogValue2 = analogRead(analogPin2);

  // apply direction adjustment for channel 1
  if(analogValue1 > 512)
  {
    MotorShield.setMotorDir(1, 1);
    pwmValue1 = ((float)analogValue1 - 512) * 0.1953; // evaluate new pwm value
  }
  else
  {
    MotorShield.setMotorDir(1, 0);
    pwmValue1 = ((float)511 - analogValue1) * 0.1953; // evaluate new pwm value
  }

  // apply direction adjustment for channel 2
  if(analogValue2 > 512)
  {
    MotorShield.setMotorDir(2, 1);
    pwmValue2 = ((float)analogValue2 - 512) * 0.1953; // evaluate new pwm value
  }
  else
  {
    MotorShield.setMotorDir(2, 0);
    pwmValue2 = ((float)511 - analogValue2) * 0.1953; // evaluate new pwm value
  }

  // apply speed adjustment
  MotorShield.setMotorSpeed(1, pwmValue1);
  MotorShield.setMotorSpeed(2, pwmValue2);

  temperature = MotorShield.getTemperature();
  Serial.println(temperature,1);        // temperature is printed with 1 decimal
}
