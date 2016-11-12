/*
Vincent Pineau  04/11/2016 Sketch Arduino du robot
*/
#include <Wire.h>
#include <Servo.h>

Servo horizon;
#define  IrNumber 5
#define debug 0 // mode debug 1=oui, 0 = non
// Constantes communes aux cartes 
#define adresseCartePuissance 0x12
#define NO_DATA 0 // sert à l'écriture sur le bus I2C sans donnée dans le registre
#define DATA_OK 1 // Protocol OK
#define maxReg 5 // Nombre de registres

// Liste des registres
#define R_ETAT 0 // Registre d'etat de la carte
  #define R_ETAT_ECHO 1 // Bit 1 : Echo (la carte fonctionne)
  #define R_ETAT_B1 2   // Bit 2 : Bouton 1 : 0 = Relache, 1 = Appuye
  #define R_ETAT_B2 3   // Bit 3 : Bouton 2 : 0 = Relache, 1 = Appuye
  #define R_ETAT_B3 4   // Bit 4 : Bouton 3 : 0 = Relache, 1 = Appuye
  #define R_ETAT_B4 5   // Bit 5 : Bouton 4 : 0 = Relache, 1 = Appuye
  #define R_ETAT_B5 6   // Bit 6 : Bouton 5 : 0 = Relache, 1 = Appuye

const int ledPin = 13; // LED 
const int key_S1_5 = 7; // Bouton de 1 a 5
long t,t1; // Variable de controle du temps
int data;
int registre;
volatile int regs[maxReg];

const byte encoder0pinAG = 2;
//A pin -> the interrupt pin 0 
const byte encoder0pinBG = 4;
//B pin -> the digital pin 4 

const byte encoder0pinAD = 3;
//A pin -> the interrupt pin 0 
const byte encoder0pinBD = 5;
//B pin -> the digital pin 4 

byte encoder0PinALastG; 
int durationG;//the number of the pulses 
boolean DirectionG;//the rotation direction
byte encoder0PinALastD; 
int durationD;//the number of the pulses 
boolean DirectionD;//the rotation direction

void wheelSpeedG() 
{ 
	int Lstate = digitalRead(encoder0pinAG); 
	if ((encoder0PinALastG == LOW) && Lstate==HIGH) 
	{ 
		int val = digitalRead(encoder0pinBG); 
		if (val == LOW && DirectionG) 
		{ 
			DirectionG = false; //Reverse 
		} else 
			if (val == HIGH && !DirectionG) 
				{ DirectionG = true; //Forward 
                                } 
	} 
	encoder0PinALastG = Lstate;
	if (!DirectionG) durationG++; else durationG--; 
}

void wheelSpeedD() 
{ 
	int Lstate = digitalRead(encoder0pinAD); 
	if ((encoder0PinALastD == LOW) && Lstate==HIGH) 
	{ 
		int val = digitalRead(encoder0pinBD); 
		if (val == LOW && DirectionD) 
		{ 
			DirectionD = false; //Reverse 
		} else 
			if (val == HIGH && !DirectionD) 
				{ DirectionD = true; //Forward 
                                } 
	} 
	encoder0PinALastD = Lstate;
	if (!DirectionD) durationD++; else durationD--; 
}


void EncoderInit() 
{ 
	DirectionG = true;//default -> Forward 
        DirectionD = true;//default -> Forward 
	pinMode(encoder0pinBG,INPUT); 
        pinMode(encoder0pinBD,INPUT);
        pinMode(encoder0pinAG,INPUT_PULLUP);
        pinMode(encoder0pinAD,INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(encoder0pinAG), wheelSpeedG, CHANGE);
        attachInterrupt(digitalPinToInterrupt(encoder0pinAD), wheelSpeedD, CHANGE);
}

int decode_bouton()
{
  int w = analogRead(key_S1_5);
  
  #define vS1 0
  #define vS2 130
  #define vS3 306
  #define vS4 478
  #define vS5 720
  if ( w < vS2/2 ) return 1;
  if ( w < (vS3+vS2)/2 ) return 2;
  if ( w < (vS4+vS3)/2 ) return 3;
  if ( w < (vS5+vS4)/2 ) return 4;
  if ( w < (1024+vS5)/2 ) return 5;
  return 0;
}


void setup() { 
    t=millis();
    regs[R_ETAT]=1;
    Serial.begin(9600);
    Wire.begin(adresseCartePuissance);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
    pinMode(ledPin, OUTPUT); // Led en sortie  
     pinMode(8,INPUT);
      pinMode(10,INPUT);
    pinMode(8,INPUT); 
    pinMode (A1, INPUT);
    pinMode (A2, INPUT);
    pinMode (A3, INPUT);
    pinMode (A4, INPUT);
    pinMode (A5, INPUT);
    EncoderInit();
}

void loop()
{
  if ((millis()-t)>10)
  {
    //Serial.print("PulseG:"); 
	//Serial.println(durationG); 
	durationG = 0; 
//Serial.print("PulseD:"); 
	//Serial.println(durationD); 
	durationD = 0;
    int b1=digitalRead(10);
    int b2=digitalRead(9);
    int b3=digitalRead(8);
    
    if (b1==0) Serial.println("Droite");
    if (b2==0) Serial.println("Milieu");
    if (b3==0) Serial.println("Gauche");
  	uint16_t value = analogRead (A1);
	uint16_t range = get_gp2d12 (value);
	/*Serial.println (value);
	Serial.print (range);
	Serial.println (" mm");
	Serial.println ();*/
    t=millis();
    int bouton = decode_bouton();
        
    if ( bouton >0 ) {
      int val;
       val=regs[R_ETAT];
        switch (bouton)
        {
          case 1 : val|=((B00000000 * 256) + B00000010); break;
          case 2 : val|=((B00000000 * 256) + B00000100); break;
          case 3 : val|=((B00000000 * 256) + B00001000); break;
          case 4 : val|=((B00000000 * 256) + B00010000); break;
          case 5 : val|=((B00000000 * 256) + B00100000); break;
        }
        regs[R_ETAT]=val;
        // un bouton est appuyé
        Serial.print("Bouton : S");
        Serial.println(bouton );
      }
    }
}

void receiveData(int byteCount){
    while(Wire.available()) {
      int d1,d2;
        registre = Wire.read();
        if (debug) Serial.print(">"); 
        if (debug) Serial.print(registre,DEC);  
        d1 = Wire.read();
        if (debug)Serial.print(",");
        if (debug)Serial.print(d1,HEX);
        d2 = Wire.read();
        if (debug)Serial.print(",");
        if (debug)Serial.print(d2,HEX);
        data=(d2<<8)|d1;
        if (debug)Serial.print(",");
        if (debug)Serial.print(data,HEX);
        if (debug)Serial.print(",");
        if (debug)Serial.println(data,DEC);
    }
}

void I2Cwrite(int d)
{
  Wire.write(d);
  if (debug) Serial.print("<");
  if (debug) Serial.print(registre,DEC);
  if (debug)Serial.print(",");
  if (debug) Serial.println(d,HEX);
}

void sendData()
{
    int i;
    I2Cwrite(regs[registre]);
    if (registre==R_ETAT) regs[registre]=DATA_OK;  
}

uint16_t get_gp2d12 (uint16_t value) {
	if (value < 10) value = 10;
	return ((67870.0 / (value - 3.0)) - 40.0);
}
