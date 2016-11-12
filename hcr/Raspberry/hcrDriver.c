/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)
# Driver basé sur la librairie wiringpi (wiringpi.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Code source du driver HCR pour Raspberry
*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "hcrDriver.h"
#define debug 1 // 1 : affichage des info de debug, 0 : pas d'affichage

etatRobot hcrEtat;
static int romeoI2C;    // Carte pilotant les IR, les bumpers et les encodeurs
static int arduinoI2C2; // Carte pilotant les moteurs


void ecrisI2C(int canalI2C, int registre, int data, char *message) // Ecriture d'une commande sur un bus I2C
{
	if (registre<0 | registre>127) 
		sprintf(message,"Erreur valeur de registre incorrecte : %d\n",registre);
	else
	{
		if (debug) sprintf(message,"writeI2C %d,R:%d,[%X]\n",canalI2C,registre,data);
		else message[0]='\0';
		wiringPiI2CWriteReg16(canalI2C,registre,data);
	}
}


int lisI2C(int canalI2C, char *message) // Lecture d'une donnée sur un bus I2C
{
	int data=wiringPiI2CRead(canalI2C);
	if (debug) sprintf(message,"readI2C %d,[%X]\n",canalI2C,data);
	else message[0]='\0';
	return data;
}

int hcrInit(char (*message)[maxNbMessage][maxLongMessage],int *nbMess) // Initialisation du pilote
{
	int i;
	*nbMess=1;
	sprintf((*message)[0],"Pilote HCR Version %s\n",hcrVersion); 
	wiringPiSetup(); // Initialisation de librairie wiringpi 
	romeoI2C = wiringPiI2CSetup(adresseI2Cromeo); // driver I2C carte des capteurs
	arduinoI2C2 = wiringPiI2CSetup(adresseI2Carduino); // driver I2C carte de puissance
	sprintf((*message)[(*nbMess)],"I2C carte capteurs :%X\n",adresseI2Cromeo); (*nbMess)++; 
	sprintf((*message)[(*nbMess)],"I2C drive moteur   :%X\n",adresseI2Carduino); (*nbMess)++; 
	for (i=0; i<hcrNbBumper; i++)
		hcrEtat.bumper[i]=0;
	for (i=0; i<hcrNbBouton; i++)
		hcrEtat.bouton[i]=0;
	hcrEtat.echoCapteur=0;
	hcrEtat.echoPuissance=0;
	return hcrOK;
}

int hcrEtatMAJ(char (*message)[maxNbMessage][maxLongMessage],int *nbMess) // Mise à jour de l'état du robot
{
	int i;
	ecrisI2C(romeoI2C, R_ETAT, NO_DATA,(*message)[0]); // Demande de lecture d'état à la carte des capteurs
	delay(pauseEcritureI2C);
	int data=lisI2C(romeoI2C,(*message)[1]);
	int bit;
    for (i=15; i>=0; i--)
	{
		bit=(((data>>i)&1)==1);
		switch (i)
		{
			case 0 : hcrEtat.echoCapteur=bit; break;
			case 1 : hcrEtat.bouton[hcrBouton1]=bit; break;
			case 2 : hcrEtat.bouton[hcrBouton2]=bit; break;
			case 3 : hcrEtat.bouton[hcrBouton3]=bit; break;
			case 4 : hcrEtat.bouton[hcrBouton4]=bit; break;
			case 5 : hcrEtat.bouton[hcrBouton5]=bit; break;
		}
	}

    hcrEtat.bumper[hcrBumperGauche]=0;
    hcrEtat.bumper[hcrBumperMilieu]=1;
    hcrEtat.bumper[hcrBumperDroit]=1;
	/*hcrEtat.bouton[hcrBouton1]=1;
	hcrEtat.bouton[hcrBouton2]=1;
	hcrEtat.bouton[hcrBouton3]=0;
	hcrEtat.bouton[hcrBouton4]=1;
	hcrEtat.bouton[hcrBouton5]=1;*/
	*nbMess=2;
	return 1;
}