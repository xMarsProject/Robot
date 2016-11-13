/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Header du driver HCR pour Raspberry
*/

#ifndef hcrDriverINCLUDED

#define hcrDriverINCLUDED
#define lonMaxChaine 255
#define hcrVersion "1.0"
#define hcrOK 1
#define hcrKO 0
#define maxNbMessage 20 // nombre de message affiché dans le log de l'interface
#define maxLongMessage 30 // longueur max des messages

#define adresseI2Cromeo 0x12 // Adresse de la carte DFRobot Romeo V1.2
#define adresseI2Carduino 0x08 // Adresse de la carte arduino pour le drive moteur

#define pauseEcritureI2C 100 // Temps d'attente en ms après une écriture sur le bus I2C
#define pauseLectureI2C 10 // Temps d'attente en ms après une écriture sur le bus I2C

#define hcrNbBumper 3
#define hcrBumperGauche 0
#define hcrBumperMilieu 1
#define hcrBumperDroit  2

#define hcrNbIR 5
#define hcrIrGauche1 0
#define hcrIrGauche2 1
#define hcrIrMilieu  2
#define hcrIrDroit2  3
#define hcrIrDroit1  4

// Constantes communes aux cartes 
#define NO_DATA 0 // sert à l'écriture sur le bus I2C sans donnée dans le registre
#define DATA_OK 2 // Protocol OK

// Liste des registres carte Roméo
#define romeoMaxReg 6 // Nombre de registres
#define romeoREG1 0 // Registre 1 (8 bits) bumper
#define romeoREG2 1 // Registre 2 (8 bits) IR gauche1
#define romeoREG3 2 // Registre 2 (8 bits) IR gauche2
#define romeoREG4 3 // Registre 2 (8 bits) IR milieu
#define romeoREG5 4 // Registre 2 (8 bits) IR droit1
#define romeoREG6 5 // Registre 2 (8 bits) IR droit2

typedef struct etatRobot etatRobot;
struct etatRobot
{
	int echoCapteur;
	int echoPuissance;
    int bumper[hcrNbBumper]; // bumper avant du robot
    int IR[hcrNbIR]; // capteurs infrarouges
};

int hcrInit(char (*message)[maxNbMessage][maxLongMessage],int *nbMess); // Initialisation du pilote
int hcrEtatMAJ(char (*message)[maxNbMessage][maxLongMessage],int *nbMess); // Mise à jour de l'état du robot
void ecrisI2C(int canalI2C, int registre, int data, char *message); // Ecriture d'une commande sur un bus I2C
int lisI2C(int canalI2C, char *message); // Lecture d'une donnée sur un bus I2C

#endif