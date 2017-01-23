/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
*/

#ifndef piloteINCLUDED
#define piloteINCLUDED

#include <math.h>

#define NbTickParTR 1322
#define DiametreRoue 13
#define CirconferenceRoue (double)M_PI*DiametreRoue

#define NbBumper 3
#define BumperGauche 0
#define BumperMilieu 1
#define BumperDroit  2

#define NbIR 5
#define IrGauche1 0
#define IrGauche2 1
#define IrMilieu  2
#define IrDroit2  3
#define IrDroit1  4

#define true 1
#define false 0

typedef struct etatCapteur etatCapteur;
struct etatCapteur
{
	int arretUrgence;
    int bumper[NbBumper]; // bumper avant du robot
    int IR[NbIR]; // capteurs infrarouges
};

typedef struct etatMoteur etatMoteur;
struct etatMoteur
{
	int dernierEtatG; // Dernier etat encodeur moteur gauche
	int nbTickG;      // Nombre de tick encodeur moteur gauche
	int sensLuG;      // Sens de la rotation actuelle moteur gauche
	int dernierEtatD; // Dernier etat encodeur moteur droit
	int nbTickD;      // Nombre de tick encodeur moteur droit
	int sensLuD;      // Sens de la rotation actuelle moteur droit
	
	int puissanceG;   // Puissance Moteur Gauche
	int puissanceD;   // Puissance Moteur Droit
	int sensG;        // Sens de rotation Moteur Gauche
	int sensD;        // Sens de rotation Moteur Doit
};

// Port du Raspberry

#define encoder0pinAG 2 // Port A de l'interruption encodeur moteur gauche 
#define encoder0pinAD 21 // Port A de l'interruption encodeur moteur droit
#define encoder0pinBG 22 // Port B de l'encodeur moteur gauche
#define encoder0pinBD 0 // Port B de l'encodeur moteur droit

#define adresseI2Ccapteur 0x12 // Adresse de la carte des capteurs
#define adresseI2Cpuissance 0x08 // Adresse de la carte de puissance

//Initialisation du pilote
int init(void);

/*************************************************************************
*               Fonctions de pilotage des encodeurs
**************************************************************************/

// fonction de calcul du nombre de tick de la roue gauche
// appele par l'interruption de l'encodeur gauche
void tickGauche();
// fonction de calcul du nombre de tick de la roue droite
// appele par l'interruption de l'encodeur droite
void tickDroit();

/*************************************************************************
*               Fonctions de pilotage des capteurs
**************************************************************************/

// fonction permettant de convertir un nombre compris entre in_min et in_max
// en un nombre compris entre out_min et out_max
// ser pour décoder un nombre passé sur 8 bit (0 à 255) sur le bus I2C
long map(long x, long in_min, long in_max, long out_min, long out_max);
void decodeRegistre1(int data);
// Mise à jour de l'état des capteurs
int majCapteur (void );

/*************************************************************************
*               Fonction de pilotage des moteurs
**************************************************************************/

// Mise à jour de l'état des moteurs
int majMoteur(void);
int avance(int puissanceG, int puissanceD);
int recule(int puissanceG, int puissanceD);

//Normalise les puissances pour ne pas depasser les valeurs maximales et minimales
void normalise(int *puissanceG, int *puissanceD);

//donne le nb de tour de roue en fonction de la vitesse en kmH et de la durée de
// l'échantillonnage en milliseconde
int calculNbTR(double kmH, int echantillionMS);

/*************************************************************************
*               Fonction de pilotage du bus 2IC
**************************************************************************/

// Ecriture d'une commande sur un bus I2C
void ecrisI2C(int canalI2C, int registre, int data);

// Lecture d'une donnée sur un bus I2C
int lisI2C(int canalI2C);

#endif