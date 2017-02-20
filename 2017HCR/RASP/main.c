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
#include <string.h>
#include <stdlib.h>  

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 

#include "pilote.h"
#include "main.h"

extern etatMoteur moteur;
extern etatCapteur capteur;

double VitesseG=0;
double VitesseD=0;
int SensG=0;
int SensD=0;
int socket_desc , android_socket;
tControle Controle[mxXmy];
struct sockaddr_in server , client;

pthread_t asservissement; // thread d'asservissement

// Asservissement en vitesse roue par roue

void *th_asservissement( void *rien )
{
    int duree=100; // Durée de l'échantillonnage
	int consigneG; // consigne pour le Nb de tick à droite
	int consigneD; // consigne pour le Nb de tick à gauche
    int NbTickG=0; // nombre de tick réél à gauche
    int NbTickD=0; // nombre de tick réél à droite
    int ecartTickG; // écart entre la consigne et le nb réél de tick à gauche
    int ecartTickD; // écart entre la consigne et le nb réél de tick à droite
    int ecartPuissanceG; // écart entre la puissance moteur gauche actuelle et la puissance corrigée
    int ecartPuissanceD; // écart entre la puissance moteur droit actuelle et la puissance corrigée
    double correctionG; // correction en pourcentage à appliquer au pont en H pour la roue gauche
    double correctionD; // correction en pourcentage à appliquer au pont en H pour la roue droite
    unsigned long int cumulEcartG=0; // Cumul des écarts du nb de tick à Gauche
    unsigned long int cumulEcartD=0; // Cumul des écarts du nb de tick à Droite
    
    
    int puissanceG=0; // puissance demandée au pont en H (de 0 à 255) pour la roue gauche
    int puissanceD=0; // puissance demandée au pont en H (de 0 à 255) pour la roue droite
    
    do {
    	// Réinitialisation du nb de tick à gauche et à droite
    	moteur.nbTickG=0; 
	  	moteur.nbTickD=0;
	  	
	  	// Calcul des consignes en fonction de la vitesse demandée
	  	consigneG=calculNbTck(VitesseG<0?VitesseG*-1:VitesseG, duree);
        consigneD=calculNbTck(VitesseD<0?VitesseD*-1:VitesseD, duree);
        
    	delay(duree);
    	if (majBumper()!=0 && VitesseG>0 && VitesseD>0)
    	{
	  		printf("Arret d'urgence\n");
	  		stop();
	  		resetMoteur();
	  	}
	  	
	  	// Lecture du nb réél de tick gauche et droit
	  	NbTickG=abs(moteur.nbTickG);
      	NbTickD=abs(moteur.nbTickD);
      	cumulEcartG=cumulEcartG+NbTickG;
      	cumulEcartD=cumulEcartD+NbTickD;
      	
      	// Calcul de l'écart du nb de tick entre la consigne et le réél 
	  	ecartTickG=consigneG-NbTickG;
      	ecartTickD=consigneD-NbTickD;
      	
      	// Calcul des pourcentages d'écart des nb de tick entre la consigne et le réél
      	correctionG=(double)ecartTickG/(double)consigneG;
      	correctionD=(double)ecartTickD/(double)consigneD;
      	
      	// Application des pourcentages aux puissances actuelles (la correction est signée)
      	ecartPuissanceG=puissanceG*correctionG;
    	ecartPuissanceD=puissanceD*correctionD;
    	
    	// Calcul des nouvelles puissances
    	if (puissanceG==0 && correctionG>0) // Cas ou la roue et à l'arrêt et on veut la démarrer
    		puissanceG=125; // Puissance moyenne
    	else
    		puissanceG+=ecartPuissanceG;
    		
    	if (puissanceD==0 && correctionD>0) // Cas ou la roue et à l'arrêt et on veut la démarrer
    		puissanceD=125; // Puissance moyenne
    	else
    		puissanceD+=ecartPuissanceD;
    			
    	normalise(&puissanceG, &puissanceD); // Normalisation des puissances

    	// Application des changements aux moteurs
      	controleMoteur(puissanceG,SensG,puissanceD,SensD);
    } while ( 1 );
    ferme();
}

// Initialisation de la matrice des vitesses et des directions

void initMatrice( )
{
	int x,y,i;
	
	double Vmax=3;
    double yVmax;
    
    int dy=my/2+1;
    int dx=mx/2+1;
    
    double pasx=(Vmax/((double)dx));
    double pasy=(Vmax/((double)dy));

    for (y=0; y<my; y++)
    {
        if (y+1<dy) yVmax=Vmax-pasy*y;
        if (y+1>dy) yVmax=pasy*(y+2-((double)dy));
        if (y+1==dy) yVmax=0;
    	pasx=(yVmax/((double)dx));
    	
        for (x=1; x<mx+1; x++)
    	{
    	    i=y*mx+x-1;
			Controle[i].sensG=(y+1>dy)?0:1;        
		    Controle[i].sensD=(y+1>dy)?0:1;
		    
		    if (x<dx)
		    {
		    	Controle[i].vitesseG=(x)*pasx;
		    	Controle[i].vitesseD=yVmax;
		    }
		    if (x>dx)
		    {
		    	Controle[i].vitesseG=yVmax;
		    	Controle[i].vitesseD=yVmax-(x-dx)*pasx;
		    }
		    if (x==dx)
		    {
		    	Controle[i].vitesseG=yVmax;
		    	Controle[i].vitesseD=yVmax;
		    }
    	}    
    } 
}

//Affichage de la matrice des vitesses et des directions (pour le debug)

void afficheMatrice ()
{
    int x,y,i;
	for (y=0; y<my; y++)
    {    	
        for (x=1; x<mx+1; x++)
    	{
    	    i=y*mx+x-1;
    		printf("(%0.3f;%0.3f)",Controle[i].vitesseG,Controle[i].vitesseD);
    	}
    	printf("\n");
    }
}


// Initialisation de la connexion réseau

int initReseau ( int port )
{
    //Création de la socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Echec de la création de la socket réseau\n");
        return 0;
    }
     
    //Configuration de la socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
     
    //Binding du port
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("port occupé : %d\n", port);
        return 0;
    }
    printf("Robot en écoute sur le port %d\n",port);
     
    //Port en écoute
    listen(socket_desc , 3);
     
    printf("Attente de la connexion ...\n");
    int c = sizeof(struct sockaddr_in);
    android_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (android_socket<0)
    {
        printf("Echec de la connexion\n");
        close(android_socket);
   		close(socket_desc);
    	return 0;
    }
     
    printf("Android connecté\n");
	return 1;
}

// Lecture d'une commande envoyée par le téléphone

int lectureCommande()
{
	int nbOctetLu;
    char message[200] , client_message[2000];

    int i=0;
    int j,fin=0;
    int stat=1;
    
    while(1) 
    {
     	nbOctetLu = recv(android_socket , client_message , 2000 , 0);
      	if(nbOctetLu == 0)
    	{
        	printf("Android déconnecté\n");
        	break;
    	}
    	if(nbOctetLu == -1)
    	{
        	printf("Echec de la lecture de la commande\n");
        	stat=0;
        	break;
    	}
    	if (nbOctetLu >0)
    	{
     		for(j=0; j<nbOctetLu; j++)
        	{
     			if (client_message[j]=='F') 
     			{
					printf("[Fin]\n");
    				fin=1;
    				break;
    			}
    			if (client_message[j]=='S') 
     			{
					printf("[Stop]\n");
					VitesseG=0;
                    VitesseD=0;
    			}
    			if (client_message[j]==';')
    			{
    				if (j-3>=0)
    				{ 
    					int z=(client_message[j-3]-48)*100+(client_message[j-2]-48)*10+client_message[j-1]-48-1;
    					printf("{[z=%d] [VG=%0.3f] [VD=%0.3f] [SG=%d] [SD=%d]}\n",
    						z+1,Controle[z].vitesseG,Controle[z].vitesseD,Controle[z].sensG,Controle[z].sensD);
    			    	VitesseG=Controle[z].vitesseG;
                    	VitesseD=Controle[z].vitesseD;
                    	SensG=Controle[z].sensG;
                    	SensD=Controle[z].sensD;
    				}
    			}
     		}
     		if (fin) break;
     	}
    }
    return stat;
}

int main(int argc, char *argv[])
{
	printf("Pilote Robot V1.0");
    initMatrice();
    afficheMatrice();    
	init();
    resetMoteur();
	if (pthread_create(&asservissement,NULL, th_asservissement,NULL )==-1)
	{
    	printf("Erreur en lancement du programme d'asservissement\n");
    	ferme();
     	return 0;
	}
    
    if (!initReseau(8888))
    {
    	printf("Erreur lors de l'initialisation réseau\n");
    	ferme();
     	return 0;
    }
    
    if (!lectureCommande())
    {
    	printf("Erreur lors de la lecture des commandes\n");
    }
        
    close(android_socket);
    close(socket_desc);
    ferme();
    printf("Fin du programme\n");
}