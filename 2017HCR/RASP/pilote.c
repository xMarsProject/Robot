/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
*/

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <math.h>

#include "pilote.h"

// globales

static int capteurI2C;    // Carte pilotant les IR, les bumpers et les encodeurs
static int puissanceI2C;  // Carte pilotant les moteurs
static int boussoleI2C;   // La boussole
etatCapteur capteur;
etatMoteur moteur;
struct termios clavier;

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
	capteur.arretUrgence=0;
	
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

    boussoleI2C=wiringPiI2CSetup(adresseI2Cboussole);
    if (boussoleI2C==-1)
    {
        printf("La boussole n'est pas initialisée\n");
        return 0;
    }
    if (wiringPiI2CWriteReg8(boussoleI2C,0x02,0x00)==-1) // mode de lecture en continu
    {
        printf("La boussole n'est pas initialisée\n");
        return 0;
    }

	/*
	if (initTerminal(&clavier)==-1) // configuration du terminal
    {
        printf("Le clavier n'est pas initialisé ...\n");
        return 0;
    }*/

	return 1;
}

// Fermeture du pilote

int ferme(void)
{
	stop();
	//fermeTerminal (&clavier);
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
// sert pour décoder un nombre passé sur 8 bit (0 à 255) sur le bus I2C
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Mise à jour de l'état des capteurs

int majCapteur (void )
{
    int bit,i,data;
	ecrisI2C(capteurI2C, 0, 0); // Demande l'état des infra rouges
	data=lisI2C(capteurI2C); // Récupération de l'état (0 = Ok, 1 = obstacle <20 cm)
	for (i=7; i>=0; i--)
	{
		bit=(((data>>i)&1)==1);
		switch (i)
		{
			case 0 : capteur.IR[IrGauche1]=bit; break;
			case 1 : capteur.IR[IrGauche2]=bit; break;
			case 2 : capteur.IR[IrMilieu]=bit; break;
			case 3 : capteur.IR[IrDroit1]=bit; break;
			case 4 : capteur.IR[IrDroit2]=bit; break;
		}
	}	
	return 1;
}

// Mise à jour de l'état des bumpers

int majBumper(void)
{
    int bit,i,data,ok;
	ecrisI2C(puissanceI2C, 5, 1); // Demande de lecture des bumpers et arret urgence
	data=lisI2C(puissanceI2C); // Récupération de l'état des bumpers et arrêt d'urgence
	if (data>15 || data<0 ) return 0; // Parfois il y a des valeurs erratiques
	for (i=7; i>=0; i--)
	{
		bit=(((data>>i)&1)==1);
		switch (i)
		{
			case 0 : capteur.arretUrgence=bit; break;
			case 1 : capteur.bumper[BumperGauche]=bit; break;
			case 2 : capteur.bumper[BumperMilieu]=bit; break;
			case 3 : capteur.bumper[BumperDroit]=bit; break;
		}
	}
	return capteur.arretUrgence;
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

//donne le nb de tick de roue encodeuse en fonction de la vitesse en kmH et de la durée de
// l'échantillonnage en milliseconde

int calculNbTck(double kmH, int echantillionMS)
{
    double dNbTickParTR=NbTickParTR;
    double dCirconferenceRoue=CirconferenceRoue;
    double dechantillionMS=echantillionMS;
	return dNbTickParTR/dCirconferenceRoue*kmH*100000/3600/1000*dechantillionMS;
}

int controleMoteur(int puissanceG, int sensG, int puissanceD, int sensD)
{
	normalise(&puissanceG, &puissanceD);
	moteur.puissanceG=puissanceG;   // Puissance Moteur Gauche
	moteur.puissanceD=puissanceD;   // Puissance Moteur Droit
	moteur.sensG=sensG;        // Sens de rotation Moteur Gauche
	moteur.sensD=sensD;        // Sens de rotation Moteur Doit
	majMoteur();
}

int stop()
{
	moteur.puissanceG=0;   // Puissance Moteur Gauche
	moteur.puissanceD=0;   // Puissance Moteur Droit
	majMoteur();
	return 1;
}

int resetMoteur()
{
	ecrisI2C(puissanceI2C, 6, 1); // Sortie mode urgence
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

/**********************************************************************************************************
 fonction de gestion du clavier
***********************************************************************************************************/

/* cette fonction reconfigure le terminal, et stocke */
/* la configuration initiale a l'adresse prev */

int initTerminal (struct termios *prev)
{
    struct termios new;
    if (tcgetattr(fileno(stdin),prev)==-1)
    {
        perror("tcgetattr");
        return -1;
    }
    new.c_iflag=prev->c_iflag;
    new.c_oflag=prev->c_oflag;
    new.c_cflag=prev->c_cflag;
    new.c_lflag=0;
    new.c_cc[VMIN]=1;
    new.c_cc[VTIME]=0;
    if (tcsetattr(fileno(stdin),TCSANOW,&new)==-1)
    {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

/* cette fonction restaure le terminal avec la configuration stockee a l'adresse prev */
int fermeTerminal (struct termios *prev)
{
    return tcsetattr(fileno(stdin),TCSANOW,prev);
}

/**********************************************************************************************************
 Compass Management
***********************************************************************************************************/

int majBoussole( void )
{
    short int x,y,z;
    int xm,xl,ym,yl,zm,zl;
    double angle;

    xm=wiringPiI2CReadReg8(boussoleI2C,0x03);
    xl=wiringPiI2CReadReg8(boussoleI2C,0x04);
    zm=wiringPiI2CReadReg8(boussoleI2C,0x05);
    zl=wiringPiI2CReadReg8(boussoleI2C,0x06);
    ym=wiringPiI2CReadReg8(boussoleI2C,0x07);
    yl=wiringPiI2CReadReg8(boussoleI2C,0x08);

    x=(xm<<8) | xl;
    y=(ym<<8) | yl;
    z=(zm<<8) | zl;
    angle=atan2((double)x,(double)y)*180/M_PI+180;
    capteur.angle=angle;
    	//printf("%d %d %d %f\n",x,y,z,angle);
    return (int)angle;
}