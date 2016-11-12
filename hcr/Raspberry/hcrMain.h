/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Header du programme de test HCR pour Raspberry
*/

#define tailleMaxIHM 100 // taille maximum du masque écran de l'IHM
#define pulse 100 // nombre de milliseconde entre chaque passage de boucle
#define pulseIHM 200 // nombre de centième de seconde avant rafraichissement interface

extern etatRobot hcrEtat;

void afficheIHM(char (*ecran)[tailleMaxIHM][tailleMaxIHM],int NbLigne, int NbColonne);  // affiche le masque écran
int initIHM(char (*ecran)[tailleMaxIHM][tailleMaxIHM],int *NbLigne, int *NbColonne); // initialise l'écran via curses
void closeIHM(); // ferme curses et restaure l'écran  
void majIHM(char (*message)[maxNbMessage][maxLongMessage], int nbMessage); // Affiche l'état du robot dans l'IHM
void loop(); // boucle principale
void ihmSleep_ms(int milisec); // attend x millisecondes