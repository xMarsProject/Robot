/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
*/

#ifndef mainINCLUDED
#define mainINCLUDED

#define mx 11
#define my 23
#define mxXmy mx*my

typedef struct tControle tControle;
	struct tControle
	{
		double vitesseG;     // Vitesse roue gauche
		double vitesseD;     // Vitesse roue droite
		int sensG;        // Sens de rotation Moteur Gauche Sens=0 Arrière sens=1 avant
		int sensD;        // Sens de rotation Moteur Doit Sens=0 Arrière sens=1 avant
	};

void *th_asservissement( void *rien );

// Initialisation de la matrice des vitesses et des directions

void initMatrice( void );

//Affichage de la matrice des vitesses et des directions (pour le debug)

void afficheMatrice ( void );

// Initialisation de la connexion réseau

int initReseau ( int port );

// Lecture d'une commande envoyée par le téléphone

int lectureCommande();

#endif