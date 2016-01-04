

#include <SPI.h>
#include <Servo.h>

#include <IRremote.h>
#include <IRremoteInt.h>

int RECV_PIN = 8;

IRrecv irrecv(RECV_PIN);

decode_results results;

#define max_h 146
#define max_v 169
#define min_h 25
#define min_v 39
#define stp 10

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
int LeftSound=0;
char p1,p2;
char r1,r2;
 
void setup (void)
{
  irrecv.enableIRIn(); // Start the receiver
  irrecv.blink13(0);
  Serial.begin (9600);
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  horizon.attach(9);
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
} 


ISR (SPI_STC_vect)
{
  char c = SPDR;
  /*Serial.print("c=");*/
  //Serial.println(c);
/*Serial.print(";cmd=");
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
        //case 1: if (c=='1') LedOn(); else LedOff(); break;
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

void ReadLeftSoundSensor()
{
  LeftSound=analogRead(A5);
}

void ReadLeftLightSensor()
{
  LeftSound=analogRead(A4);
}

void ReadIRCapter()
{
  LeftSound=analogRead(3);
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
  Serial.println("L");
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}

void turn_R (char a,char b)             //Turn Right
{
  Serial.println("R");
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

void stop_it(void)                    //Stop
{
  Serial.println("S");
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
  Serial.println(analogRead(A5));
  delay(1000);
} 

