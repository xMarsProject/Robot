/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <math.h>

#include "pilote.h"

// globales

static int capteurI2C;    // Carte pilotant les IR, les bumpers et les encodeurs
static int puissanceI2C; // Carte pilotant les moteurs

etatCapteur capteur;
etatMoteur moteur;

//Initialisation du pilote 

int init(void)
{
    int i;
	wiringPiSetup(); // Initialisation de librairie wiringpi 
	capteurI2C = wiringPiI2CSetup(adresseI2Ccapteur); // driver I2C carte des capteurs
	puissanceI2C = wiringPiI2CSetup(adresseI2Cpuissance); // driver I2C carte de puissance
	pinMode(encoder0pinAG,INPUT);
    pinMode(encoder0pinAD,INPUT);
	pinMode(encoder0pinBG,INPUT); 
    pinMode(encoder0pinBD,INPUT);
        
    wiringPiISR(encoder0pinAG,INT_EDGE_BOTH,&tickGauche); // interruption encodeur gauche
    wiringPiISR(encoder0pinAD,INT_EDGE_BOTH,&tickDroit); // interruption encodeur droit
	capteur.echoCapteur=1;
	
	for (i=0; i<3; i++)
		capteur.bumper[i]=0;
	for (i=0; i<5; i++)
		capteur.IR[i]=0;

	moteur.dernierEtatG=0; // Dernier etat encodeur moteur gauche
	moteur.nbTickG=0;      // Nombre de tick encodeur moteur gauche
	moteur.sensLuG=0;      // Sens de la rotation actuelle moteur gauche
	moteur.dernierEtatD=0; // Dernier etat encodeur moteur droit
	moteur.nbTickD=0;      // Nombre de tick encodeur moteur droit
	moteur.sensLuD=0;      // Sens de la rotation actuelle moteur droit
	
	moteur.puissanceG=0;   // Puissance Moteur Gauche
	moteur.puissanceD=0;   // Puissance Moteur Droit
	moteur.sensG=0;        // Sens de rotation Moteur Gauche
	moteur.sensD=0;        // Sens de rotation Moteur Doit

	return 1;
}

/*************************************************************************
*               Fonctions de pilotage des encodeurs
**************************************************************************/

// fonction de calcul du nombre de tick de la roue gauche
// appele par l'interruption de l'encodeur gauche
void tickGauche() 
{ 
	int EtatGauche = digitalRead(encoder0pinAG); 
	if ((moteur.dernierEtatG == LOW) && EtatGauche==HIGH) 
	{ 
		int val = digitalRead(encoder0pinBG); 
		if (val == LOW && moteur.sensG) 
		    moteur.sensLuG = false; //Marche arriere 
		else 
		    if (val == HIGH && !moteur.sensG) 
		        moteur.sensLuG = true; //Marche avant 
	} 
	moteur.dernierEtatG = EtatGauche;
	if (!moteur.sensG) moteur.nbTickG++; else moteur.nbTickG--; 
}

// fonction de calcul du nombre de tick de la roue droite
// appele par l'interruption de l'encodeur droite
void tickDroit() 
{ 
	int EtatDroit = digitalRead(encoder0pinAD); 
	if ((moteur.dernierEtatD == LOW) && EtatDroit==HIGH) 
	{ 
		int val = digitalRead(encoder0pinBD); 
		if (val == LOW && moteur.sensD) 
		    moteur.sensLuD = false; //Marche arriere 
		else 
		    if (val == HIGH && !moteur.sensD) 
		        moteur.sensLuD = true; //Marche avant          
	} 
	moteur.dernierEtatD = EtatDroit;
	if (!moteur.sensLuD) moteur.nbTickD++; else moteur.nbTickD--; 
}

/*************************************************************************
*               Fonctions de pilotage des capteurs
**************************************************************************/

// fonction permettant de convertir un nombre compris entre in_min et in_max
// en un nombre compris entre out_min et out_max
// ser pour décoder un nombre passé sur 8 bit (0 à 255) sur le bus I2C
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Décodage des registres

void decodeRegistre1(int data)
{
	int bit;
	int i;
	for (i=7; i>=0; i--)
	{
		bit=(((data>>i)&1)==1);
		switch (i)
		{
			case 0 : capteur.echoCapteur=bit; break;
			case 1 : capteur.bumper[BumperGauche]=bit; break;
			case 2 : capteur.bumper[BumperMilieu]=bit; break;
			case 3 : capteur.bumper[BumperDroit]=bit; break;
		}
	}
}

// Mise à jour de l'état des capteurs

int majCapteur (void )
{
    int i;
	int registre;
	int data;
	int bit;
	for (registre=0; registre<6; registre++)
	{
		ecrisI2C(capteurI2C, registre, 0); 
		data=lisI2C(capteurI2C);
		switch (registre)
		{
			case 0 : decodeRegistre1(data); break;
			case 1 : capteur.IR[IrGauche1]=map(data,0,255,0,1000); break;
			case 2 : capteur.IR[IrGauche2]=map(data,0,255,0,1000); break;
			case 3 : capteur.IR[IrMilieu]=map(data,0,255,0,1000); break;
			case 4 : capteur.IR[IrDroit2]=map(data,0,255,0,1000); break;
			case 5 : capteur.IR[IrDroit1]=map(data,0,255,0,1000); break;
		}
	}
	return 1;
}

/*************************************************************************
*               Fonction de pilotage des moteurs
**************************************************************************/

// Mise à jour de l'état des moteurs

int majMoteur(void)
{
	ecrisI2C(puissanceI2C, 0, moteur.sensG); // Sens de rotation Moteur Gauche
	ecrisI2C(puissanceI2C, 1, moteur.sensD); // Sens de rotation Moteur Doit
	ecrisI2C(puissanceI2C, 2, moteur.puissanceG); // Puissance Moteur Gauche
	ecrisI2C(puissanceI2C, 3, moteur.puissanceD); // Puissance Moteur Droit
	ecrisI2C(puissanceI2C, 4, 1); // Demande de prise en compte des nouveaux paramètres
	return 1;
}

//Normalise les puissances pour ne pas dépasser les valeurs maximales et minimales
void normalise(int *puissanceG, int *puissanceD)
{
	*puissanceG=(*puissanceG>250)?250:*puissanceG;
	*puissanceG=(*puissanceG<0)?0:*puissanceG;
	*puissanceD=(*puissanceD>250)?250:*puissanceD;
	*puissanceD=(*puissanceD<0)?0:*puissanceD;
}

//donne le nb de tour de roue en fonction de la vitesse en kmH et de la durée de
// l'échantillonnage en milliseconde

int calculNbTR(double kmH, int echantillionMS)
{
	return NbTickParTR/CirconferenceRoue*kmH*100000/3600/1000*echantillionMS;
}

int avance(int puissanceG, int puissanceD)
{	
	normalise(&puissanceG, &puissanceD);
	moteur.puissanceG=puissanceG;   // Puissance Moteur Gauche
	moteur.puissanceD=puissanceD;   // Puissance Moteur Droit
	moteur.sensG=1;        // Sens de rotation Moteur Gauche
	moteur.sensD=1;        // Sens de rotation Moteur Doit
	majMoteur();
	return 1;
}

int recule(int puissanceG, int puissanceD)
{	
	normalise(&puissanceG, &puissanceD);
	moteur.puissanceG=puissanceG;   // Puissance Moteur Gauche
	moteur.puissanceD=puissanceD;   // Puissance Moteur Droit
	moteur.sensG=0;        // Sens de rotation Moteur Gauche
	moteur.sensD=0;        // Sens de rotation Moteur Doit
	majMoteur();
	return 1;
}

int stop()
{
	moteur.puissanceG=0;   // Puissance Moteur Gauche
	moteur.puissanceD=0;   // Puissance Moteur Droit
	majMoteur();
	return 1;
}

/*************************************************************************
*               Fonction de pilotage du bus 2IC
**************************************************************************/

// Ecriture d'une commande sur un bus I2C
void ecrisI2C(int canalI2C, int registre, int data) 
{
	wiringPiI2CWriteReg16(canalI2C,registre,data);
}

// Lecture d'une donnée sur un bus I2C
int lisI2C(int canalI2C) 
{
	int data=wiringPiI2CRead(canalI2C);
	return data;
}

