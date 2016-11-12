/*
Vincent Pineau  04/11/2016 Driver du robot
*/
#include <termio.h>

#define driverVersion "V1 (04-11-2016 Vincent PINEAU)"

#define adresseCartePuissance 0x12 // Adresse de la carte de puissance DFRobot Romeo V1.2
#define pauseEcritureI2C 100 // Temps d'attente après une écriture sur le bus I2C
#define pauseLectureI2C 10 // Temps d'attente après une écriture sur le bus I2C
#define debug 0 // Mode debug : 0 non, 1 : oui

// Constantes communes aux cartes 
#define NO_DATA 0 // sert à l'écriture sur le bus I2C sans donnée dans le registre
#define DATA_OK 2 // Protocol OK

#define maxReg 1 // Nombre de registre

// Liste des registres
#define R_ETAT 0
#define R_COMD 1

int init(); // Initialisation des cartes
void ecrisI2C(int canalI2C, int registre, int data); // Ecriture d'une commande sur un bus I2C
int lisI2C(int canalI2C); // Lecture d'une donnée sur un bus I2C
void lisRegistreEtat(void); // Lecture du registre d'état
int codeRegistre(int registre, int plage, int valeur);
void ecrisRegistreEtat(int registre);
void bot_close(void);