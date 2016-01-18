/* ************************************************************
 *  Robot Controller Vincent PINEAU 01/01/2016
 * ************************************************************/

#include <SPI.h>
#include <Servo.h>
#include <IRremote.h>
#include <IRremoteInt.h>

// Pin allocation
#define IR_RECIEVER  8 // IR reciever
#define SVM_HORIZON  2 // Servo Motor Horizontal
#define SVM_VERTICAL 3 // Servo Motor Vertical 
#define E1           5 //Right Motor Speed Control
#define E2           6 //Left Motor Speed Control
#define M1           4 //Right Motor Direction Control
#define M2           7 //Left Motor Direction Control
#define RASP_MISO    12 // Raspberry SPI
#define RASP_MOSI    11 // Raspberry SPI
#define RASP_SCLK    13 // Raspberry SPI
#define RASP_SS      10 // Raspberry SPI New command 
#define LFT_SND      19 // Left Sound Sensor 
#define RGT_SND      17 // Right Sound Sensor
#define LFT_LIGHT    18 // Left Light Sensor
#define RGT_LIGHT    15 // Right Light Sensor

IRrecv irrecv(IR_RECIEVER);
decode_results results;

// Servo Motor moving limit
#define max_h 146
#define max_v 169
#define min_h 25
#define min_v 39
#define stp 10

int posh=0; // Horizontal Servo Motor current position
int posv=0; // Vertical Servo Motor current position
Servo horizon;
Servo vertical;

char command = 0; // Raspberry Command 
byte param[8]; // byte data for Raspberry
int nb = 0; // Number of command read from the Raspberry

/*
 * Setup (executed once)
 */

void setup (void)
{
  irrecv.enableIRIn(); // Start the IR receiver
  Serial.begin (9600); // Setup serial (USB for debug purpose)
  //SPI setup
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  
  // Servo motor setup
  horizon.attach(SVM_HORIZON);
  vertical.attach(SVM_VERTICAL);
  
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
  
  /*
  unsigned int i=2000,j,k;
  Serial.println(i,BIN);
  j=i>>8;
  k=i<<8;
  k=k>>8;
  Serial.print(i>>8,BIN); Serial.println(i>>8);
  Serial.print((i<<8)>>8,BIN); Serial.println((i<<8)>>8);*/

} 

/*
 *  SPI interuption (one byte is ready to be read)
 */

ISR (SPI_STC_vect)
{
  char c = SPDR;
  unsigned int analog_value;
  int t;
  switch (command)
  {
  case 0:
    {// no command so c is a new command ...
      command = c;
      SPDR = 0;
      break;
    }
   case 'A':
   {// Analogic regsiter (no parameter) (return parameter 1:LFT_SND, 2:RGT_SND, 3:LFT_LIGHT, 4:RGT_LIGHT)
      nb++;
      if (nb==1)
      {
        analog_value=analogRead(LFT_SND); //Serial.print("LS"); Serial.println(analog_value);
        param[0]=(analog_value<<8)>>8; // byte 1
        param[1]=analog_value>>8; // byte 2
        analog_value=analogRead(RGT_SND); //Serial.print("RS"); Serial.println(analog_value);
        param[2]=(analog_value<<8)>>8;
        param[3]=analog_value>>8;
        analog_value=analogRead(LFT_LIGHT); //Serial.print("LL"); Serial.println(analog_value);
        param[4]=(analog_value<<8)>>8;
        param[5]=analog_value>>8;
        analog_value=analogRead(RGT_LIGHT); //Serial.print("RL"); Serial.println(analog_value);
        param[6]=(analog_value<<8)>>8;
        param[7]=analog_value>>8;
        //for (int i=0; i<8; i++)
        //  Serial.println(param[i]);
      }
      if (nb>=1 && nb<=8) SPDR=param[nb-1];
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
    case '1' : advance(255,255); break;
    case '2' : back_off(255,255); break;
    case '3' : turn_L(255,255);  break;
    case '4' : turn_R (255,255); break;
    case '5' : stop_it(); break;
  }
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
  
   if (irrecv.decode(&results)) {
    Serial.println(results.value);
    irrecv.resume(); // Receive the next value
  }
  delay(10);
} 

