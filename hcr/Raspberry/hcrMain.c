/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
#  Code source du programme de test HCR pour Raspberry
*/

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "hcrDriver.h"
#include "hcrMain.h"

int blinkIHM; // flag pour afficher le battement de coeur
WINDOW *messageIHM; // fenêtre pour l'affichage des messages

int main(int argc, char *argv[])
{
	char ecran[tailleMaxIHM][tailleMaxIHM]; // masque de l'écran de l'interface
	char message[maxNbMessage][maxLongMessage]; // messages de l'interface
	int NbLigne,NbColonne,i,nbMess;
	
	if (!initIHM(&ecran,&NbLigne,&NbColonne)) exit(1);
	
	if (hcrInit(&message,&nbMess)==hcrOK)
	{
		afficheIHM(&ecran,NbLigne,NbColonne);
		majIHM(&message, nbMess);
		loop(); // boucle principale
	}
	else printw("Erreur lors du chargement du pilote\n");
	closeIHM();
}

void afficheIHM(char (*ecran)[tailleMaxIHM][tailleMaxIHM],int NbLigne, int NbColonne)  // affiche le masque écran
{
	start_color();			
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	int ligne=0, colonne=0;
	for (ligne=1; ligne<NbLigne; ligne++)
	 for (colonne=1; colonne<NbColonne; colonne++)
	 	{
			if ((*ecran)[ligne][colonne]=='-')
			{
				mvaddch(ligne,colonne,ACS_HLINE);
			}
			else 
				{
					if ((*ecran)[ligne][colonne]=='!')
					{
						mvaddch(ligne,colonne,ACS_VLINE);
					} 	
					else
					{
						attron(COLOR_PAIR(2));
						mvaddch(ligne,colonne,(*ecran)[ligne][colonne]);
						attron(COLOR_PAIR(1));
					}
			}
		}
	refresh();
	
}

int initIHM(char (*ecran)[tailleMaxIHM][tailleMaxIHM],int *NbLigne, int *NbColonne) // initialise l'écran via curses
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	if(has_colors() == FALSE)
	{	
		closeIHM();
		printf("Votre terminal n'accepte pas les couleurs\n");
		return 0;
	}
	// Lecture du fichier du masque écran
	int ligne=0, colonne=0;
	for (ligne=0; ligne<tailleMaxIHM; ligne++)
	 for (colonne=0; colonne<tailleMaxIHM; colonne++)
	 	(*ecran)[ligne][colonne]=0;
	ligne=1;
	colonne=1;
	(*NbColonne)=1;
	FILE* fichier = NULL;
    int caractere = 0;
    fichier = fopen("ecran.txt", "r");
    if (fichier != NULL)
    {
        do
        {
            caractere = fgetc(fichier); 
            if (caractere=='\n') {*NbColonne=colonne; colonne=1; ligne++; }
            else if (caractere!=EOF) {(*ecran)[ligne][colonne++]=caractere;}
        } while (caractere != EOF); 
        *NbLigne=ligne;
        fclose(fichier);
    }
    else
    {	
		closeIHM();
		printf("Le fichier ecran.txt n'a pas été trouvé\n");
		return 0;
	}
	messageIHM=subwin(stdscr, 6, 64, 18, 2);
	scrollok(messageIHM,TRUE);
	return 1;
}

void closeIHM() // ferme curses et restaure l'écran  
{
	endwin();
}

void majIHM(char (*message)[maxNbMessage][maxLongMessage], int nbMessage) // Affiche l'état du robot dans l'IHM
{
	int i,j;
	attroff(A_REVERSE);
	// affichage du battement de coeur
	if (blinkIHM)
	{
		attron(COLOR_PAIR(2)); // affiche
		blinkIHM=0;
	}
	else
	{
		attron(COLOR_PAIR(3)); // efface
		blinkIHM=1;
	}
	mvaddch(2,2,'X');
	
	// affichage témoin carte capteur
	if (hcrEtat.echoCapteur)
		attron(COLOR_PAIR(4)); // affiche
	else attron(COLOR_PAIR(3)); // efface
	mvaddch(2,4,'C');
	
	// affichage témoin carte capteur
	if (hcrEtat.echoPuissance)
		attron(COLOR_PAIR(5)); // affiche
	else attron(COLOR_PAIR(3)); // efface
	mvaddch(2,6,'P');
	
	// affichage des bumpers
	for (j=0; j<hcrNbBumper; j++)
	{
		if (hcrEtat.bumper[j]) 
			attron(COLOR_PAIR(4)|A_REVERSE); // affiche
		else
		{
			attron(COLOR_PAIR(3)); // efface
			attroff(A_REVERSE);
		}
		for (i=0; i<15; i++)
			mvaddch(6,3+i+j*15+j*3,' ');
	}
	
	// affichage des boutons intégrés à la carte roméo
	for (j=0; j<hcrNbBouton; j++)
	{
		if (hcrEtat.bouton[j]) 
			attron(COLOR_PAIR(5)|A_REVERSE); // affiche
		else
		{
			attron(COLOR_PAIR(3)); // efface
			attroff(A_REVERSE);
		}
		for (i=0; i<11; i++)
			mvaddch(14,3+i+j*11+j*3,' ');
	}
		
	// affichage des messages
	for (j=0;j<nbMessage;j++)
		if ((*message)[j][0]!='\0')
			wprintw(messageIHM,"%s",(*message)[j]);
	refresh();
	wrefresh(messageIHM);
}

void ihmSleep_ms(int milisec) // attend x millisecondes
{
    struct timespec res;
    res.tv_sec = milisec/1000;
    res.tv_nsec = (milisec*1000000) % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &res, NULL);
}

void loop() // boucle principale
{
  int fin=0;
  int nbMess;
  char message[maxNbMessage][maxLongMessage]; // messages de l'interface
  int ch;
  struct timeval stop, start;
  gettimeofday(&start, NULL);
  
  for (;;) 
  {
  	ihmSleep_ms(pulse); // attend quelque milliseconde
    if ((ch = getch()) == ERR) 
    {
		gettimeofday(&stop, NULL); // lecture de l'heure
    	if ((stop.tv_usec - start.tv_usec)>pulseIHM) // toutes les X centième de secondes
        {
			hcrEtatMAJ(&message,&nbMess);
     		majIHM(&message,nbMess);
			gettimeofday(&start, NULL);
    	}  
    }
    else 
    {
    	switch (ch)
        {
           case KEY_UP        : printw("^\n"); break;
           case KEY_DOWN      : printw("v\n"); break;
           case KEY_LEFT      : printw("<\n"); break;
           case KEY_RIGHT     : printw(">\n"); break;
           case KEY_BACKSPACE : fin=1; break;
        }
    }
    if (fin) break;
  }
}