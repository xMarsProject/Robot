/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Code source du sketch arduino pour le drive moteur de microrobot (http://www.microbot.it)
3  Dual DC Motor Shield - Product Code: MR007-001.1
*/

// Configuration port moteur
int E1 = 5;     //M1 Contrôle de vitesse
int E2 = 6;     //M2 Contrôle de vitesse
int M1 = 4;    //M1 contrôle du sens de rotation
int M2 = 7;    //M1 contrôle du sens de rotatio
int urgence = 0; // Mode urgence : 0=non, 1=oui

#define bumperGauche  8 // Bumper de gauche
#define bumperMilieu  9 // Bumper du milieu
#define bumperDroit  10 // Bumper de droite

#include <Wire.h>

#define adresseCartePuissance 0x08 // Adresse 2IC
#define NO_DATA 0 // sert à l'écriture sur le bus I2C sans donnée dans le registre
#define DATA_OK 1 // Protocol OK
#define timeSleep 100// nombre de milliseconde entre chaque passe de mesure

// Liste des registres
#define maxReg 8 // Nombre de registres
#define REG1 0 // Registre 1 (8 bits) sens moteur gauche
#define REG2 1 // Registre 2 (8 bits) sens moteur droit
#define REG3 2 // Registre 3 (8 bits) puissance moteur gauche
#define REG4 3 // Registre 4 (8 bits) puissance moteur gauche
#define REG5 4 // Registre 5 (8 bits) application des changements
#define REG6 5 // Registre 6 (8 bits) Lecture des bumpers
#define REG7 6 // Registre 7 (8 bits) réinitialisation carte

// variables de controle de la communication
int data; // Donne envoyee par le raspberry
int registre; // Registre adresse par le raspberry
volatile int regs[maxReg]; // Tableau des valeurs des registres envoye au raspberry

// variable de controle des bumpers
int chocBumperGauche;
int chocBumperMilieu;
int chocBumperDroit;

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
   Serial.println(urgence,DEC);
  if (urgence==0 && pMoteurGauche>=0 && pMoteurGauche<=255 && pMoteurDroit>=0 && pMoteurDroit<=255)
  {
    analogWrite (E1,pMoteurGauche);      // vitesse moteur 1
    if (sensMoteurGauche) digitalWrite(M1,HIGH);  else digitalWrite(M1,LOW);
    analogWrite (E2,pMoteurDroit);       // vitesse moteur 2
    if (sensMoteurDroit) digitalWrite(M2,HIGH); else digitalWrite(M2,LOW);
  }
}

/*************************************************************************
*       Fonction de pilotage des bumpers
**************************************************************************/

// initialisation des ports pour les bumpers
void bumperInit()
{
    chocBumperGauche=0;
    chocBumperMilieu=0;
    chocBumperDroit=0;
    pinMode(bumperGauche,INPUT);
    pinMode(bumperMilieu,INPUT); 
    pinMode(bumperDroit,INPUT);
}

void lectureBumper()
{
    chocBumperGauche=digitalRead(bumperGauche);
    chocBumperMilieu=digitalRead(bumperMilieu);
    chocBumperDroit=digitalRead(bumperDroit);
    if ((!chocBumperGauche+!chocBumperMilieu+!chocBumperDroit) >0)
    {
      urgence=1; // Arrêt d'urgence
      analogWrite (E1,0);  
      analogWrite (E2,0);     
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
    Wire.onRequest(sendData);
}

// fonction appelee des que le raspberry veut lire des donnees
void sendData()
{
    I2Cwrite(regs[registre]);
}

// sous routine pour ecrire sur le bus I2C
void I2Cwrite(int d)
{
  Wire.write(d);
}

// fonction appelee des que le raspberry envoi des donnees
void receiveData(int byteCount){
    while(Wire.available()) {
        int d1,d2;
        registre = Wire.read();
        d1 = Wire.read();
        d2 = Wire.read();
        data=(d2<<8)|d1; // passage de 2x8 bits  1x16 bits
        Serial.println(registre,DEC);
        switch (registre)
        {
          case REG1 : regs[REG1]=data; break; //sens moteur G
          case REG2 : regs[REG2]=data; break; //sens moteur D
          case REG3 : regs[REG3]=data; break; //puissance moteur G
          case REG4 : regs[REG4]=data; break; //puissance moteur D
          case REG5 : vitesseMoteur(); break; //Application modif
          case REG7 : urgence=0; break; //sortie du mode d'arrêt d'urgence
        }
    }
}

// fonction de compactage des donnees binaire sur 16 bit
void encodageRegistre()
{
     int val=urgence;
     if (!chocBumperGauche) val|=B00000010;
     if (!chocBumperMilieu) val|=B00000100;
     if (!chocBumperDroit)  val|=B00001000;
     regs[REG6]=val;    
}

void loop() {
  delay(10);
  lectureBumper();
  encodageRegistre();
}


