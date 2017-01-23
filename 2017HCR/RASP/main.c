/*
# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version 1.0
#  Vincent PINEAU 10/11/2016
*/


#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <pthread.h>

#include "pilote.h"
extern etatMoteur moteur;

double VitesseG=0;
double VitesseD=0;
int SensG=1;
int SensD=1;

pthread_t asservissement; // thread d'asservissement

void *th_asservissement( void *rien )
{
    int duree=100;
	int puissanceG;
	int puissanceD;
    int NbTickG=0;
    int NbTickD=0;
    int nbTourRoue=150;
    double ecartG;
    double ecartD;
    do {
    	NbTickG=0;
    	NbTickD=0;
    	moteur.nbTickG=0;
	  	moteur.nbTickD=0;
	  	puissanceG=calculNbTR(VitesseG<0?VitesseG*-1:VitesseG, duree);
        puissanceD=calculNbTR(VitesseD<0?VitesseD*-1:VitesseD, duree);
        //printf("%d %d\n",puissanceG,puissanceD);
	  	controleMoteur(puissanceG,SensG,puissanceD,SensD);
    	delay(100);
    	if (majBumper()!=0 && VitesseG>0 && VitesseD>0)
    	{
	  		printf("Arret d'urgence\n");
	  		VitesseG=0;
	  		VitesseD=0;
	  	}
    	NbTickG+=abs(moteur.nbTickG);
      	NbTickD+=abs(moteur.nbTickD);
      	ecartG=((double)(NbTickG-nbTourRoue)/nbTourRoue)*puissanceG*-1;
      	ecartD=((double)(NbTickD-nbTourRoue)/nbTourRoue)*puissanceD*-1;
        puissanceG+=(int)ecartG;
      	puissanceD+=(int)ecartD;
      	normalise(&puissanceG, &puissanceD);
    } while ( 1 );
}

int main(int argc, char *argv[])
{
	init();
	if (pthread_create(&asservissement,NULL, th_asservissement,NULL )==-1)
     printf("Erreur en lancement du programme d'asservissement\n");
	int c;
	float pas=0.25;
	int avant=1;
	VitesseG=0;
    VitesseD=0;
	while(1)
    {
        c=getc(stdin);
        if (c==32) // Barre espace
        {
          VitesseD=VitesseG=0;
          SensG=SensD=1;
          resetMoteur();
        }
        if (c==27)
        {
            c=getc(stdin);
            if (c==91)
            {
                c=getc(stdin);
                switch(c)
                {
                case 68 : // Gauche Sens=0 Arrière sens=1 avant
                    if (SensD!=0 || SensG!=0)
                    {
                	if (VitesseD<VitesseG)
                	{
                		VitesseD+=pas;
                		VitesseD=(VitesseD>=2.75)?2.75:VitesseD;
                		SensD=(VitesseD>=0)?1:0;
                	} else
                	{
                    	VitesseG-=pas;
                    	VitesseG=(VitesseG<=-2.75)?-2.75:VitesseG;
                    	VitesseG=VitesseG<0?0:VitesseG;
						SensG=1;
					}
					}
                    break;
                case 67 : // Droite
                	if (SensD!=0 || SensG!=0)
                    {
                	if (VitesseG<VitesseD)
                	{
                		VitesseG+=pas;
                		VitesseG=(VitesseG>=2.75)?2.75:VitesseG;
                		SensG=1;
                	} else
                	{
                    	VitesseD-=pas;
                    	VitesseD=(VitesseD<=-2.75)?-2.75:VitesseD;
						VitesseD=VitesseD<0?0:VitesseD;
						SensD=1;
					}
					}
                    break;
                case 65 : // Haut
                    VitesseG+=pas;
                    VitesseD+=pas;
                    VitesseD=(VitesseD>=2.75)?2.75:VitesseD;
                    VitesseG=(VitesseG>=2.75)?2.75:VitesseG;
                    SensD=(VitesseD>=0)?1:0;
                    SensG=(VitesseG>=0)?1:0;
                    break;
                case 66 : // Bas
                    VitesseG-=pas;
                    VitesseD-=pas;
                    VitesseD=(VitesseD<=-2.75)?-2.75:VitesseD;
                    VitesseG=(VitesseG<=-2.75)?-2.75:VitesseG;
                    SensD=(VitesseD>=0)?1:0;
                    SensG=(VitesseG>=0)?1:0;
                    break;
                }
            }
        }
        printf("%d %d %f %f\n",SensG,SensD,VitesseG,VitesseD);
        if (c==127) break; // ESC
    }
    ferme();
}
