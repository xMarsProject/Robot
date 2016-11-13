/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Code source du sketch arduino pour la carte Romeo DF Robot
*/

#include <Wire.h>

#define timeSleep 50 // nombre de milliseconde entre chaque passe de mesure
#define debug 0 // mode debug 1=oui, 0 = non

#define adresseCartePuissance 0x12 // Adresse 2IC

#define NO_DATA 0 // sert à l'écriture sur le bus I2C sans donnée dans le registre
#define DATA_OK 1 // Protocol OK

// Liste des registres
#define maxReg 6 // Nombre de registres
#define REG1 0 // Registre 1 (8 bits) bumper
#define REG2 1 // Registre 2 (8 bits) IR gauche1
#define REG3 2 // Registre 2 (8 bits) IR gauche2
#define REG4 3 // Registre 2 (8 bits) IR milieu
#define REG5 4 // Registre 2 (8 bits) IR droit1
#define REG6 5 // Registre 2 (8 bits) IR droit2

// Affectation des ports de l'arduino
// Les capteurs infra rouge de de gauche a droite

#define IRGauche1    A6
#define IRGauche2    A7 
#define IRMilieu     A0 
#define IRDroite2    A1
#define IRDroite1    A3

#define encoder0pinAG 2 // Port A de l'interruption encodeur moteur gauche 
#define encoder0pinAD 3 // Port A de l'interruption encodeur moteur droit
#define encoder0pinBG 4 // Port B de l'encodeur moteur gauche
#define encoder0pinBD 5 // Port B de l'encodeur moteur droit
#define bumperGauche  8 // Bumper de gauche
#define bumperMilieu  9 // Bumper du milieu
#define bumperDroit  10 // Bumper de droite
#define led          13 // LED pour le battement de coeur

// variables de controle de la communication
int data; // Donne envoyee par le raspberry
int registre; // Registre adresse par le raspberry
volatile int regs[maxReg]; // Tableau des valeurs des registres envoye au raspberry

// variables de controle des encodeurs
byte encoder0PinALastG; // Dernier etat encodeur moteur gauche
int durationG;          // Nombre de tick encodeur moteur gauche
boolean DirectionG;     // Sens de la rotation moteur gauche
byte encoder0PinALastD; // Dernier etat encodeur moteur droit
int durationD;          // Nombre de tick encodeur moteur droit
boolean DirectionD;     // Sens de la rotation moteur droit

// variables de controles des infrarouges
uint16_t distanceIRGauche1;
uint16_t distanceIRGauche2;
uint16_t distanceIRMilieu;
uint16_t distanceIRDroit1;
uint16_t distanceIRDroit2;

// variable de controle des bumpers
int chocBumperGauche;
int chocBumperMilieu;
int chocBumperDroit;

// configuration (execute une seule fois automatiquement a chaque boot de la carte)
void setup() { 
    Serial.begin(9600);
    pinMode(led, OUTPUT); // Led en sortie  
    iRInit();
    encoderInit();
    bumperInit();
    raspberryInit();
}

/*************************************************************************
*               Fonctions de pilotage des encodeurs
**************************************************************************/

// fonction de calcul du nombre de tick de la roue gauche
// appele par l'interruption de l'encodeur gauche
void wheelSpeedG() 
{ 
	int Lstate = digitalRead(encoder0pinAG); 
	if ((encoder0PinALastG == LOW) && Lstate==HIGH) 
	{ 
		int val = digitalRead(encoder0pinBG); 
		if (val == LOW && DirectionG) 
		    DirectionG = false; //Marche arriere 
		else 
		    if (val == HIGH && !DirectionG) 
		        DirectionG = true; //Marche avant 
	} 
	encoder0PinALastG = Lstate;
	if (!DirectionG) durationG++; else durationG--; 
}

// fonction de calcul du nombre de tick de la roue droite
// appele par l'interruption de l'encodeur droite
void wheelSpeedD() 
{ 
	int Lstate = digitalRead(encoder0pinAD); 
	if ((encoder0PinALastD == LOW) && Lstate==HIGH) 
	{ 
		int val = digitalRead(encoder0pinBD); 
		if (val == LOW && DirectionD) 
		    DirectionD = false; //Marche arriere 
		else 
		    if (val == HIGH && !DirectionD) 
		        DirectionD = true; //Marche avant          
	} 
	encoder0PinALastD = Lstate;
	if (!DirectionD) durationD++; else durationD--; 
}

// Initialisation des encodeurs
void encoderInit() 
{ 
	DirectionG = true;//Par defaut marche avant
        DirectionD = true;//Par defaut marche arriere
        durationG = 0; 
	durationD = 0;
	pinMode(encoder0pinBG,INPUT); 
        pinMode(encoder0pinBD,INPUT);
        pinMode(encoder0pinAG,INPUT_PULLUP);
        pinMode(encoder0pinAD,INPUT_PULLUP);
        // Association des fonctions aux interruptions
	attachInterrupt(digitalPinToInterrupt(encoder0pinAG), wheelSpeedG, CHANGE);
        attachInterrupt(digitalPinToInterrupt(encoder0pinAD), wheelSpeedD, CHANGE);
}

/*************************************************************************
*       Fonction de pilotage de la communication avec le raspbery
**************************************************************************/

// Initialisation du bus I2C
void raspberryInit()
{
    int i;
    regs[REG1]=DATA_OK; // Flag etat ok
    for(i=1;i<maxReg;i++) regs[i]=0; // on met les autres registres a zero
    Wire.begin(adresseCartePuissance);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
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
    }
}

// fonction appelee des que le raspberry veut lire des donnees
void sendData()
{
    int i;
    I2Cwrite(regs[registre]);
    Serial.println(registre,DEC);
    Serial.println(regs[registre],HEX);
    Serial.println();
}

// sous routine pour ecrire sur le bus I2C
void I2Cwrite(int d)
{
  Wire.write(d);
  if (debug) Serial.print("<");
  if (debug) Serial.print(registre,DEC);
  if (debug) Serial.print(",");
  if (debug) Serial.println(d,HEX);
}

/*************************************************************************
*       Fonction de pilotage des capteurs infra rouge
**************************************************************************/

// configuration des ports pour la lecture infra rouge
void iRInit()
{
    pinMode (IRGauche1, INPUT); // Infrarouge extreme gauche
    pinMode (IRGauche2, INPUT);
    pinMode (IRMilieu, INPUT); // Infragrouge du milieu
    pinMode (IRDroite2, INPUT);
    pinMode (IRDroite1, INPUT);// infra rouge extreme droite
}

// lecture infrarouge de tous les capteurs
void lectureIR()
{
    // lecture tension + conversion en cm + passage sur 8 bit avec la fonction map
    uint16_t value;
    value = analogRead (IRGauche1);
    distanceIRGauche1 = convertTensionCm8bit (value);
    
    value = analogRead (IRGauche2);
    distanceIRGauche2 = convertTensionCm8bit (value);
   
    value = analogRead (IRMilieu);
    distanceIRMilieu = convertTensionCm8bit (value);
    
    value = analogRead (IRDroite1);
    distanceIRDroit1 = convertTensionCm8bit (value);
    
    value = analogRead (IRDroite2);
    distanceIRDroit2 = convertTensionCm8bit (value);

}

// sous routine de conversion de la distance infrarouge en centimteres et mappage sur 8bit
uint16_t convertTensionCm8bit (uint16_t value) {
	if (value < 10) value = 10;
	return map(((67870.0 / (value - 3.0)) - 40.0)/10,0,1000,0,255);
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
}

/*************************************************************************
*       debuggage
**************************************************************************/

void afficheData()
{
    Serial.println("-------------------------------");
    Serial.print("durationG         = "); Serial.println(durationG);          // Nombre de tick encodeur moteur gauche
    Serial.print("durationD         = "); Serial.println(durationD);          // Nombre de tick encodeur moteur droit
    Serial.println();
    
    // variables de controles des infrarouges
    Serial.print("distanceIRGauche1 = "); Serial.print(distanceIRGauche1); Serial.println("cm");
    Serial.print("distanceIRGauche2 = "); Serial.print(distanceIRGauche2); Serial.println("cm");
    Serial.print("distanceIRMilieu  = "); Serial.print(distanceIRMilieu); Serial.println("cm");
    Serial.print("distanceIRDroit1  = "); Serial.print(distanceIRDroit1); Serial.println("cm");
    Serial.print("distanceIRDroit2  = "); Serial.print(distanceIRDroit2); Serial.println("cm");
    Serial.println();
    
    // variable de controle des bumpers
    Serial.print("chocBumperGauche  = "); Serial.println(chocBumperGauche);
    Serial.print("chocBumperMilieu  = "); Serial.println(chocBumperMilieu);
    Serial.print("chocBumperDroit   = "); Serial.println(chocBumperDroit);
    Serial.println();
}

/*************************************************************************
*       programme principal execute en boucle
**************************************************************************/

// fonction de compactage des donnees binaire sur 16 bit
void encodageRegistre()
{
     int val;
                            val =B00000001;
     if (!chocBumperGauche) val|=B00000010;
     if (!chocBumperMilieu) val|=B00000100;
     if (!chocBumperDroit)  val|=B00001000;
     regs[REG1]=val;
     regs[REG2]=distanceIRGauche1;
     regs[REG3]=distanceIRGauche2;
     regs[REG4]=distanceIRMilieu;
     regs[REG5]=distanceIRDroit1;
     regs[REG6]=distanceIRDroit2;
}


void loop()
{
      delay(timeSleep);
      lectureIR();
      lectureBumper();
      if (debug) afficheData();
      encodageRegistre();
}
