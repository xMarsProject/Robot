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
	for (i=0; i<hcrNbIR; i++)
		hcrEtat.IR[i]=0;
	hcrEtat.echoCapteur=0;
	hcrEtat.echoPuissance=0;
	return hcrOK;
}

// fonction permettant de convertir un nombre compris entre in_min et in_max
// en un nombre compris entre out_min et out_max
// ser pour décoder un nombre passé sur 8 bit (0 à 255) sur le bus I2C
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void decodeRegistre1(int data)
{
	int bit;
	int i;
	for (i=7; i>=0; i--)
	{
		bit=(((data>>i)&1)==1);
		switch (i)
		{
			case 0 : hcrEtat.echoCapteur=bit; break;
			case 1 : hcrEtat.bumper[hcrBumperGauche]=bit; break;
			case 2 : hcrEtat.bumper[hcrBumperMilieu]=bit; break;
			case 3 : hcrEtat.bumper[hcrBumperDroit]=bit; break;
		}
	}
}

int hcrEtatMAJ(char (*message)[maxNbMessage][maxLongMessage],int *nbMess) // Mise à jour de l'état du robot
{
	int i;
	int registre;
	int data;
	int bit;
	*nbMess=0;
	for (registre=0; registre<romeoMaxReg; registre++)
	{
		ecrisI2C(romeoI2C, registre, NO_DATA,(*message)[(*nbMess)++]); 
		delay(pauseEcritureI2C);
		data=lisI2C(romeoI2C,(*message)[(*nbMess)++]);
		switch (registre)
		{
			case romeoREG1 : decodeRegistre1(data); break;
			case romeoREG2 : hcrEtat.IR[hcrIrGauche1]=map(data,0,255,0,1000); break;
			case romeoREG3 : hcrEtat.IR[hcrIrGauche2]=map(data,0,255,0,1000); break;
			case romeoREG4 : hcrEtat.IR[hcrIrMilieu]=map(data,0,255,0,1000); break;
			case romeoREG5 : hcrEtat.IR[hcrIrDroit2]=map(data,0,255,0,1000); break;
			case romeoREG6 : hcrEtat.IR[hcrIrDroit1]=map(data,0,255,0,1000); break;
		}
	}
	return 1;
}