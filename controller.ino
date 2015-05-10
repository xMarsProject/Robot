#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDRESS 0x12
int dataIn=0;
int dataOut=0;
int cmd=0;
boolean next=false;

Servo horizon;
Servo vertical;
const int ledPin = 13;
int pos = 0;

void setup() {
  pinMode(ledPin, OUTPUT); // Led en sortie
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(in);
  Wire.onRequest(out);
  
  horizon.attach(10);
  for(pos = 0; pos <= 85; pos += 1) 
  {                                   
    horizon.write(pos);              
    delay(15);                       
  }
   vertical.attach(11);
  for(pos = 0; pos <= 88; pos += 1)  
  {                                   
    vertical.write(pos);             
    delay(15);                      
  }     
}

void loop() {
  delay(100);
}

void LedOn() {
  digitalWrite(ledPin,HIGH); // On
}

void LedOff() {
  digitalWrite(ledPin,LOW); // Off
}


void debug(char *mess,int cmd)
{
  Serial.print(mess);
  Serial.println(cmd);
}

void out()
{
    Wire.write(dataOut);
    //debug("Donnee envoyee :",dataOut);
}

void in(int byteCount)
{
    while(Wire.available())
    {
      dataIn=Wire.read();
      //debug("Donnee lue :",dataIn);
      if (!next)
      {
        switch(dataIn)
        {
          case 2 : LedOn(); next=false; break; 
          case 3 : LedOff(); next=false; break;
          case 4 : cmd=dataIn; next=true; break; 
          case 5 : cmd=dataIn; next=true; break;
        }
      }
      else
      {
        next=false;
        switch(cmd)
        {
          case 4 : debug("Angle horizontal :",dataIn); horizon.write(dataIn); break;
          case 5 : debug("angle vertical",dataIn); vertical.write(dataIn); break;
        }
      }
      dataOut=dataIn;
    }  
}


