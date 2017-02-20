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
#include <stdlib.h>    //write

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr

#include "pilote.h"
extern etatMoteur moteur;
extern etatCapteur capteur;

double VitesseG=0;
double VitesseD=0;
int SensG=0;
int SensD=0;

pthread_t asservissement; // thread d'asservissement

// programme autonome : tentative d'asservissement en ligne droite
void *th_asservissement( void *rien )
{
	init();
    resetMoteur();
    delay(1000);
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
        //printf("CG=%d CD=%d ",consigneG,consigneD);
        //printf("PG=%d PD=%d ",puissanceG, puissanceD);
    	delay(duree);
    	if (majBumper()!=0 && VitesseG>0 && VitesseD>0)
    	{
	  		printf("Arret d'urgence\n");
	  		stop();
	  		break;
	  	}
	  	
	  	// Lecture du nb réél de tick gauche et droit
	  	NbTickG=abs(moteur.nbTickG);
      	NbTickD=abs(moteur.nbTickD);
      	cumulEcartG=cumulEcartG+NbTickG;
      	cumulEcartD=cumulEcartD+NbTickD;
      	//printf("TG=%d TD=%d ",NbTickG,NbTickD);
      	
      	// Calcul de l'écart du nb de tick entre la consigne et le réél 
	  	ecartTickG=consigneG-NbTickG;
      	ecartTickD=consigneD-NbTickD;
      	//printf("ETG=%d ETD=%d ",ecartTickG,ecartTickD);
      	
      	// Calcul des pourcentages d'écart des nb de tick entre la consigne et le réél
      	correctionG=(double)ecartTickG/(double)consigneG;
      	correctionD=(double)ecartTickD/(double)consigneD;
      	//printf("%%G=%f %%D=%f ",correctionG,correctionD);
      	
      	// Application des pourcentages aux puissances actuelles (la correction est signée)
      	ecartPuissanceG=puissanceG*correctionG;
    	ecartPuissanceD=puissanceD*correctionD;
    	//printf("EPG=%d EPD=%d ",ecartPuissanceG,ecartPuissanceD);
    	
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
    	//printf("PG=%d PD=%d ",puissanceG, puissanceD);
    	
    	//printf("CG=%d CD=%d\n",cumulEcartG, cumulEcartD);
    	// Application des changements aux moteurs
      	controleMoteur(puissanceG,SensG,puissanceD,SensD);
    } while ( 1 );
    ferme();
}

/*

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
	  	puissanceG=calculNbTck(VitesseG<0?VitesseG*-1:VitesseG, duree);
        puissanceD=calculNbTck(VitesseD<0?VitesseD*-1:VitesseD, duree);
	  	controleMoteur(puissanceG,SensG,puissanceD,SensD);
    	delay(duree);
    	if (majBumper()!=0 && VitesseG>0 && VitesseD>0)
    	{
	  		printf("Arret d'urgence\n");
	  		VitesseG=0;
	  		VitesseD=0;
	  	}
	  	majCapteur();
	  	majBoussole();
    	NbTickG+=abs(moteur.nbTickG);
      	NbTickD+=abs(moteur.nbTickD);
      	ecartG=((double)(NbTickG-nbTourRoue)/nbTourRoue)*puissanceG*-1;
      	ecartD=((double)(NbTickD-nbTourRoue)/nbTourRoue)*puissanceD*-1;
        puissanceG+=(int)ecartG;
      	puissanceD+=(int)ecartD;
      	normalise(&puissanceG, &puissanceD);
      	//printf("%d %d %f %f %d %d\n",NbTickG,NbTickD, ecartG, ecartD, puissanceG, puissanceD);
    } while ( 1 );
}*/

int Exemple5()
{
    int socket_desc , new_socket , c, x,y,i;
    struct sockaddr_in server , client;
    int mx=11;
    int my=23;
    double Vmax=3;
    double yVmax;
        
    typedef struct tControle tControle;
	struct tControle
	{
		double vitesseG;     // Vitesse roue gauche
		double vitesseD;     // Vitesse roue droite
		int sensG;        // Sens de rotation Moteur Gauche Sens=0 Arrière sens=1 avant
		int sensD;        // Sens de rotation Moteur Doit Sens=0 Arrière sens=1 avant
	};
    
    tControle Controle[253];
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
    for (y=0; y<my; y++)
    {    	
        for (x=1; x<mx+1; x++)
    	{
    	    i=y*mx+x-1;
    		printf("(%0.3f;%0.3f)",Controle[i].vitesseG,Controle[i].vitesseD);
    	}
    	printf("\n");
    }
	init();
	if (pthread_create(&asservissement,NULL, th_asservissement,NULL )==-1)
     printf("Erreur en lancement du programme d'asservissement\n");
    delay(50);
    resetMoteur();
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 0;
    }
    puts("bind done V3");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (new_socket<0)
    {
        perror("accept failed");
        close(new_socket);
   		close(socket_desc);
    	return 0;
    }
     
    puts("Connection accepted");
    
    int read_size;
    char message[200] , client_message[2000];
     
     
    //Receive a message from client
    i=0;
    int j,fin=0;
    
    while(1) 
    {
     read_size = recv(new_socket , client_message , 2000 , 0);
      if(read_size == 0)
    {
        puts("Client disconnected");
        break;
    }
    if(read_size == -1)
    {
        perror("recv failed");
        break;
    }
    if (read_size >0)
    {
     i++;
     //printf("(%d)\n",i);
     for(j=0; j<read_size; j++)
        {
     		//printf("%c",client_message[j]);
     		if (client_message[j]=='F') 
     			{
					puts("Fin\n");
    				fin=1;
    				break;
    			}
    		if (client_message[j]=='S') 
     			{
					puts("Stop\n");
					VitesseG=0;
                    VitesseD=0;
    			}
    		if (client_message[j]==';')
    		{
    			if (j-3>=0)
    			{ 
    				int z=(client_message[j-3]-48)*100+(client_message[j-2]-48)*10+client_message[j-1]-48-1;
    				printf("Zone %d VG=%0.3f VD=%0.3f\n",z+1,Controle[z].vitesseG,Controle[z].vitesseD);
    			    VitesseG=Controle[z].vitesseG;
                    VitesseD=Controle[z].vitesseD;
                    SensG=Controle[z].sensG;
                    SensD=Controle[z].sensD;
                    printf("%d",SensD);
    			}
    		}
     	}
     	if (fin) break;
     }
    }
    close(new_socket);
    close(socket_desc);
    ferme();
}

// programme autonome : tentative d'asservissement en ligne droite
void Exemple4()
{
	init();
    resetMoteur();
    delay(1000);
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
    
    VitesseG=2;
    VitesseD=2;
    do {
    	// Réinitialisation du nb de tick à gauche et à droite
    	moteur.nbTickG=0; 
	  	moteur.nbTickD=0;
	  	
	  	// Calcul des consignes en fonction de la vitesse demandée
	  	consigneG=calculNbTck(VitesseG<0?VitesseG*-1:VitesseG, duree);
        consigneD=calculNbTck(VitesseD<0?VitesseD*-1:VitesseD, duree);
        printf("CG=%d CD=%d ",consigneG,consigneD);
        printf("PG=%d PD=%d ",puissanceG, puissanceD);
    	delay(duree);
    	if (majBumper()!=0 && VitesseG>0 && VitesseD>0)
    	{
	  		printf("Arret d'urgence\n");
	  		stop();
	  		break;
	  	}
	  	
	  	// Lecture du nb réél de tick gauche et droit
	  	NbTickG=abs(moteur.nbTickG);
      	NbTickD=abs(moteur.nbTickD);
      	cumulEcartG=cumulEcartG+NbTickG;
      	cumulEcartD=cumulEcartD+NbTickD;
      	printf("TG=%d TD=%d ",NbTickG,NbTickD);
      	
      	// Calcul de l'écart du nb de tick entre la consigne et le réél 
	  	ecartTickG=consigneG-NbTickG;
      	ecartTickD=consigneD-NbTickD;
      	printf("ETG=%d ETD=%d ",ecartTickG,ecartTickD);
      	
      	// Calcul des pourcentages d'écart des nb de tick entre la consigne et le réél
      	correctionG=(double)ecartTickG/(double)consigneG;
      	correctionD=(double)ecartTickD/(double)consigneD;
      	printf("%%G=%f %%D=%f ",correctionG,correctionD);
      	
      	// Application des pourcentages aux puissances actuelles (la correction est signée)
      	ecartPuissanceG=puissanceG*correctionG;
    	ecartPuissanceD=puissanceD*correctionD;
    	printf("EPG=%d EPD=%d ",ecartPuissanceG,ecartPuissanceD);
    	
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
    	printf("PG=%d PD=%d ",puissanceG, puissanceD);
    	
    	printf("CG=%d CD=%d\n",cumulEcartG, cumulEcartD);
    	// Application des changements aux moteurs
      	controleMoteur(puissanceG,SensG,puissanceD,SensD);
    } while ( 1 );
    ferme();
}

// programme autonome : tentative d'asservissement avec la boussole (marche pas bien)
void Exemple3()
{
	int puissanceG=0;
	int puissanceD=0;
	int angleConsigne;
	int ecart;
	int reel;
	int i;
	init();
	SensG=SensD=1;
    VitesseD=VitesseG=0;
    resetMoteur();
    delay(1000);
	angleConsigne=majBoussole();
	i=0;
	do {
		controleMoteur(puissanceG,SensG,puissanceD,SensD);
		delay(1000);
		majCapteur();
		majBumper();
	  	reel=majBoussole();
	  	ecart=angleConsigne-reel;
	  	printf("(%d)consigne=%d reel=%d ecart=%d\n",i++,angleConsigne,reel,ecart);
	}
	while ( !capteur.IR[IrGauche2] &&
		!capteur.IR[IrGauche1] &&
		!capteur.IR[IrMilieu] &&
		!capteur.IR[IrDroit1] &&
		!capteur.arretUrgence &&
		!capteur.bumper[BumperGauche] &&
		!capteur.bumper[BumperMilieu] &&
		!capteur.bumper[BumperDroit] );
		if (capteur.IR[IrGauche1]) printf("G1\n");
		if (capteur.IR[IrGauche2]) printf("G2\n");
		if (capteur.IR[IrMilieu]) printf("M\n");
		if (capteur.IR[IrDroit1]) printf("D1\n");
		if (capteur.IR[IrDroit2]) printf("D2\n");
    ferme();
}

// programme autonome : avance et s'arrête quand on trouve un obstacle
void Exemple2()
{
	init();
	if (pthread_create(&asservissement,NULL, th_asservissement,NULL )==-1)
     printf("Erreur en lancement du programme d'asservissement\n");
    delay(50);
    // On avance tout droit et on s'arrête si on trouve un obstacle
    resetMoteur();
    delay(50);
    VitesseG=2; // 1,5 km roue gauche
    VitesseD=2; // 1,5 km roue droite
    
    while ( !capteur.IR[IrGauche2] &&
		!capteur.IR[IrGauche2] &&
		!capteur.IR[IrMilieu] &&
		!capteur.IR[IrDroit1] &&
		!capteur.IR[IrDroit2] && 
		!capteur.arretUrgence &&
		!capteur.bumper[BumperGauche] &&
		!capteur.bumper[BumperMilieu] &&
		!capteur.bumper[BumperDroit] ) 
	delay(100);
	SensG=SensD=1;
    VitesseD=VitesseG=0;
    resetMoteur();
    if (!capteur.arretUrgence)
    {
		printf("Obstacle à moins de 20 cm ! infra rouge actif :\n");
		if (capteur.IR[IrGauche1]) printf("G1\n");
		if (capteur.IR[IrGauche2]) printf("G2\n");
		if (capteur.IR[IrMilieu]) printf("M\n");
		if (capteur.IR[IrDroit1]) printf("D1\n");
		if (capteur.IR[IrDroit2]) printf("D2\n");
		printf("Arret avant collision\n");
    }
    delay(1000);
    ferme();
}

//* Pilotage à l'aide du clavier
void Exemple1()
{
	init();
	if (pthread_create(&asservissement,NULL, th_asservissement,NULL )==-1)
     printf("Erreur en lancement du programme d'asservissement\n");
         resetMoteur();
    delay(50);
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
        //printf("%d %d %f %f\n",SensG,SensD,VitesseG,VitesseD);
        if (c==127) break; // ESC
    }
    ferme();
}

int main(int argc, char *argv[])
{
	//Exemple1(); // Pilotage clavier
	//Exemple2(); // Programme autonome
	//Exemple3(); // test avec la boussole
	//Exemple4(); // test en ligne droite avec asservissement
	Exemple5(); // test avec une connection TCP
}