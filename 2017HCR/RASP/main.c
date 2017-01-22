/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
*/


#include <stdio.h>
#include <time.h>

#include "pilote.h"
extern etatMoteur moteur;

int main(int argc, char *argv[])
{
	printf("%d",calculNbTR(1.65, 100));
	FILE* fichier = NULL;
    fichier = fopen("log.txt", "w");
    init();
    int puissanceG=114;
    int puissanceD=114;
    int NbTickG=0;
    int NbTickD=0;
    int nbTourRoue=150;
    double ecartG;
    double ecartD;
    int k=0;
   do {
    	NbTickG=0;
    	NbTickD=0;
    	moteur.nbTickG=0;
	  	moteur.nbTickD=0;
    	avance(puissanceG,puissanceD);
    	delay(100);
    	k+=100;
    	NbTickG+=abs(moteur.nbTickG);
      	NbTickD+=abs(moteur.nbTickD);
      	//fprintf(fichier,"A %d\t%d\t%d\t%d\n",puissanceG,puissanceD,NbTickG,NbTickD);
      	printf("A %d\t%d\t%d\t%d\n",puissanceG,puissanceD,NbTickG,NbTickD);
      	ecartG=((double)(NbTickG-nbTourRoue)/nbTourRoue)*puissanceG*-1;
      	ecartD=((double)(NbTickD-nbTourRoue)/nbTourRoue)*puissanceD*-1;
        puissanceG+=(int)ecartG;
      	puissanceD+=(int)ecartD;
      	normalise(&puissanceG, &puissanceD);
      	printf("B %d\t%d\t%d\t%d\n",puissanceG,puissanceD,NbTickG,NbTickD);
      	//fprintf(fichier,"B %d\t%d\t%d\t%d\n",puissanceG,puissanceD,NbTickG,NbTickD);
    } while ( NbTickG!=nbTourRoue || NbTickD!=nbTourRoue );
    printf("\n%d\n",k);
	fclose(fichier);
	stop();
}

/*
int main(int argc, char *argv[])
{
int i;
    int j;
    int NbTickG=0;
    int NbTickD=0;
	init();
	FILE* fichier = NULL;
    fichier = fopen("log.txt", "w");
    i=114;
    avance(i);
    for (j=1;j<40;j++)
    {
	  	moteur.nbTickG=0;
	  	moteur.nbTickD=0;
      	delay(100);
      	NbTickG+=abs(moteur.nbTickG);
      	NbTickD+=abs(moteur.nbTickD);
	  fprintf(fichier,"%d\t%d\t%d\t%d\n",i,j,NbTickG,NbTickD);
	  NbTickG=0;
      NbTickD=0;
	}
	avance(0);
	do {
    moteur.nbTickG=0;
	moteur.nbTickD=0;
    NbTickG=0;
    NbTickD=0;
	delay(100);
	NbTickG+=abs(moteur.nbTickG);
    NbTickD+=abs(moteur.nbTickD);
	fprintf(fichier,"%d\t%d\t%d\t%d\n",i,j,NbTickG,NbTickD);
	} while ((NbTickG+NbTickD)>0);
	fclose(fichier);
	stop();
}
*/

/*
int main(int argc, char *argv[])
{
int i;
    int j;
    int NbTickG=0;
    int NbTickD=0;
	init();
	FILE* fichier = NULL;
    fichier = fopen("log.txt", "w");
    for (i=114; i<200; i++)
    {
	  avance(i);
	  	moteur.nbTickG=0;
	  	moteur.nbTickD=0;
      	delay(100);
      	NbTickG+=abs(moteur.nbTickG);
      	NbTickD+=abs(moteur.nbTickD);
	  fprintf(fichier,"%d\t%d\t%d\t%d\n",i,j,NbTickG,NbTickD);
	  NbTickG=0;
      NbTickD=0;
	}
	fclose(fichier);
	stop();
}*/
/*
int i;
    int j;
    int NbTickG;
    int NbTickD;
	init();
    for (i=70; i<255; i++)
    {
	  avance(i);
      for (j=0; j<4; j++)
      {
	  	moteur.nbTickG=0;
	  	moteur.nbTickD=0;
      	delay(100);
      	NbTickG+=abs(moteur.nbTickG);
      	NbTickD+=abs(moteur.nbTickD);
      }
	  printf("%d\t%d\t%d\t%d\n",i,j,NbTickG/4,NbTickD/4);
	  NbTickG=0;
      NbTickD=0;
	}
*/

/*
    init();
    avance(255);
    delay(5000);
    stop();
*/