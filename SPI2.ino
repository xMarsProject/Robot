#include <SPI.h>
#include <Servo.h>
#include <Wire.h>

#define max_h 146
#define max_v 169
#define min_h 25
#define min_v 39
#define stp 10

#define DEVICE (0x53) // Device address as specified in data sheet 

byte _buff[6];

char POWER_CTL = 0x2D;	//Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;	//X-Axis Data 0
char DATAX1 = 0x33;	//X-Axis Data 1
char DATAY0 = 0x34;	//Y-Axis Data 0
char DATAY1 = 0x35;	//Y-Axis Data 1
char DATAZ0 = 0x36;	//Z-Axis Data 0
char DATAZ1 = 0x37;	//Z-Axis Data 1

#define ALPHA 0.5

const int ledPin = 13;
Servo horizon;
Servo vertical;
char command = 0;
int posh=0;
int posv=0;
int nb=0;
int x=0,y=0,z=0;
const int E1 = 5; //M1 Speed Control
const int E2 = 6; //M2 Speed Control
const int M1 = 4; //M1 Direction Control
const int M2 = 7; //M1 Direction Control

char p1,p2;
char r1,r2;
 
void setup (void)
{
  Serial.begin (9600);
  pinMode(MISO, OUTPUT);
 // pinMode(ledPin, OUTPUT);
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  horizon.attach(2);
  vertical.attach(3);
  
  for(posh = 0; posh <= 85; posh += 1) 
  {                                   
    horizon.write(posh);              
    delay(15);                       
  }
  for(posv = 0; posv <= 88; posv += 1)  
  {                                   
    vertical.write(posv);             
    delay(15);                      
  }
  Wire.begin();        // join i2c bus (address optional for master)
  
  //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
  writeTo(DATA_FORMAT, 0x01);
  //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);
} 


ISR (SPI_STC_vect)
{
  char c = SPDR;
  /*Serial.print("c=");
  Serial.print(c);
Serial.print(";cmd=");
  Serial.print(command);
  Serial.print(";nb=");
  Serial.println(nb);*/
  switch (command)
  {
  case 0:
    {// no command so c is a new command ...
      command = c;
      SPDR = 0;
      break;
    }
  case 'L':
   { // led (parameter 1=on, 0=off)
      nb++;
      switch(nb)
      {
        case 1: if (c=='1') LedOn(); else LedOff(); break;
      }
      break;
   }
   case 'H': 
     {// horizontal servo motor (parameter 1=right, 0=left)
        nb++;
        switch(nb)
        {
          case 1: if (c=='1') posh=(posh>=max_h)?max_h:(posh+stp); else posh=(posh<=min_h)?min_h:(posh-stp);  horizon.write(posh); break;
        } 
        break;
     }
    case 'V': 
    { // vertical servo motor (parameter 1=up, 0=down)
      nb++;
      switch(nb)
      {
        case 1: if (c=='1') posv=(posv>=max_v)?max_v:(posv+stp); else posv=(posv<=min_v)?min_v:(posv-stp);  vertical.write(posv); break;
      }
      break;
    }
    case 'A': 
    {// accelerometer (no parameter) (return parameter 1:x, 2:y, 3:z)
      nb++;
      switch(nb)
      {
        case 1: SPDR=x; break;
        case 2: SPDR=y; break;
        case 3: SPDR=z; break;
      }
      break;
    }
    case '1' : advance(255,255); break;
    case '2' : back_off(255,255); break;
    case '3' : turn_L(255,255);  break;
    case '4' : turn_R (255,255); break;
    case '5' : stop_it(); break;
  }
} 

void LedOn() {
  digitalWrite(ledPin,HIGH); // On
}

void LedOff() {
  digitalWrite(ledPin,LOW); // Off
}

void readAccel() {
  uint8_t howManyBytesToRead = 6;
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345

  // each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  // thus we are converting both bytes in to one int
  x = (((int)_buff[1]) << 8) | _buff[0];   
  y = (((int)_buff[3]) << 8) | _buff[2];
  z = (((int)_buff[5]) << 8) | _buff[4];
}

void writeTo(byte address, byte val) {
  Wire.beginTransmission(DEVICE); // start transmission to device 
  Wire.write(address);             // send register address
  Wire.write(val);                 // send value to write
  Wire.endTransmission();         // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[]) {
  Wire.beginTransmission(DEVICE); // start transmission to device 
  Wire.write(address);             // sends address to read from
  Wire.endTransmission();         // end transmission

  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num);    // request 6 bytes from device

  int i = 0;
  while(Wire.available())         // device may send less than requested (abnormal)
  { 
    _buff[i] = Wire.read();    // receive a byte
    i++;
  }
  Wire.endTransmission();         // end transmission
}

void advance(char a,char b)          //Move forward
{
  analogWrite (E1,a);      //PWM Speed Control
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  

void back_off (char a,char b)          //Move backward
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);   
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

void turn_L (char a,char b)             //Turn Left
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}

void turn_R (char a,char b)             //Turn Right
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

void stop_it(void)                    //Stop
{
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);      
}  

void loop (void)
{
  if (digitalRead (SS) == HIGH)
  {
    command = 0;
    nb=0;
  } 
  delay(100);
  readAccel();
} 
