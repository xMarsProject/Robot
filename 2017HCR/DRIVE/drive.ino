/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Code source du sketch arduino pour le drive moteur de microrobot (http://www.microbot.it)
3  Dual DC Motor Shield - Product Code: MR007-001.1
*/

//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction Control

///For previous Romeo, please use these pins.
//int E1 = 6;     //M1 Speed Control
//int E2 = 9;     //M2 Speed Control
//int M1 = 7;    //M1 Direction Control
//int M2 = 8;    //M1 Direction Control

#include <Wire.h>

#define debug 0// mode debug 1=oui, 0 = non
#define adresseCartePuissance 0x08 // Adresse 2IC
#define NO_DATA 0 // sert à l'écriture sur le bus I2C sans donnée dans le registre
#define DATA_OK 1 // Protocol OK
#define timeSleep 100// nombre de milliseconde entre chaque passe de mesure

// Liste des registres
#define maxReg 5 // Nombre de registres
#define REG1 0 // Registre 1 (8 bits) sens moteur gauche
#define REG2 1 // Registre 2 (8 bits) sens moteur droit
#define REG3 2 // Registre 3 (8 bits) puissance moteur gauche
#define REG4 3 // Registre 4 (8 bits) puissance moteur gauche
#define REG5 4 // Registre 5 (8 bits) application des changements

// variables de controle de la communication
int data; // Donne envoyee par le raspberry
int registre; // Registre adresse par le raspberry
volatile int regs[maxReg]; // Tableau des valeurs des registres envoye au raspberry

void raspberryInit();

// configuration (execute une seule fois automatiquement a chaque boot de la carte)
void setup() {
    int i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);  
  raspberryInit();
  Serial.begin(9600);          
}

void vitesseMoteur()
{

      int sensMoteurGauche=regs[REG1];
      int sensMoteurDroit=regs[REG2];
      int pMoteurGauche=regs[REG3];
      int pMoteurDroit=regs[REG4];

if (pMoteurGauche>=0 || pMoteurGauche<=255 || pMoteurDroit>=0 || pMoteurDroit<=255)
{

  analogWrite (E1,pMoteurGauche);      //PWM Speed Control
  if (sensMoteurGauche)
  digitalWrite(M1,HIGH);  
  else
  digitalWrite(M1,LOW);
  
  analogWrite (E2,pMoteurDroit);
  if (sensMoteurDroit)
    digitalWrite(M2,HIGH);
  else
    digitalWrite(M2,LOW);
  Serial.print("G="); Serial.print(" "); Serial.print(sensMoteurGauche);Serial.print(" "); Serial.print(pMoteurGauche);
  Serial.print("D="); Serial.print(" "); Serial.print(sensMoteurDroit); Serial.print(" "); Serial.println(pMoteurDroit);
}
else
{
  Serial.print("Error G="); Serial.print(" "); Serial.print(sensMoteurGauche);Serial.print(" "); Serial.print(pMoteurGauche);
  Serial.print("Error D="); Serial.print(" "); Serial.print(sensMoteurDroit); Serial.print(" "); Serial.println(pMoteurDroit);
}
   
}

/*************************************************************************
*       Fonction de pilotage de la communication avec le raspbery
**************************************************************************/

// Initialisation du bus I2C
void raspberryInit()
{
    int i;
    for(i=0;i<maxReg;i++) regs[i]=0; // on met tous les registres a zero
    Wire.begin(adresseCartePuissance);
    Wire.onReceive(receiveData);
}

// fonction appelee des que le raspberry envoi des donnees
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
        data=(d2<<8)|d1; // passage de 2x8 bits  1x16 bits
        if (debug)Serial.print(",");
        if (debug)Serial.print(data,HEX);
        if (debug)Serial.print(",");
        if (debug)Serial.println(data,DEC);
        switch (registre)
        {
          case REG1 : regs[REG1]=data; break; //sens moteur G
          case REG2 : regs[REG2]=data; break; //sens moteur D
          case REG3 : regs[REG3]=data; break; //puissance moteur G
          case REG4 : regs[REG4]=data; break; //puissance moteur D
          case REG5 : vitesseMoteur(); break; //Application modif
        }
    }
}

void loop() {
  delay(10);
}


