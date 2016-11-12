/*
Vincent Pineau  04/11/2016 Driver du robot
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <ncurses.h>
#include "driver.h"

int canalI2C1;
int canalI2C2;
int echo1;
int bouton1;
int bouton2;
int bouton3;
int bouton4;
int bouton5;

struct termios bot_keyboard;

// Initialisation des cartes
int init()
{
	wiringPiSetup();
		initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	canalI2C1 = wiringPiI2CSetup(adresseCartePuissance);
	canalI2C2 = wiringPiI2CSetup(8);
	printw("Canal I2C carte de puissance : %X\n",canalI2C1);
	printw("Adresse carte de puissance   : %X\n",adresseCartePuissance);
	printw("Driver %s chargé\n",driverVersion);
	lisRegistreEtat();
	if (echo1) printw("Carte de puissance Ok\n");
	else 
	{
		printw("Erreur la carte de puissance ne répond pas !\n"); 
		return 0;
	}

	
	/*
	if (bot_cnf_terminal(&bot_keyboard)==-1) // configure the terminal to use the keybord key by key
    {
        printf("Keyboard can't be initialized ...\n");
        return 0;
    }*/
	return 1;
}

void bot_close(void)
{
    //if (bot_rest_terminal(&bot_keyboard)==-1)
    //    printf("Error during keyboard restoration\n");
    endwin();
}

// Ecriture d'une commande sur un bus I2C
void ecrisI2C(int canalI2C, int registre, int data)
{
	if (registre<0 | registre>127) 
		printw("Erreur valeur de registre incorrecte : %d\n",registre);
	else
	{
		if (debug) printw("writeI2C %d,R:%d,[%X]\n",canalI2C,registre,data);
		wiringPiI2CWriteReg16(canalI2C,registre,data);
		wiringPiI2CWriteReg16(canalI2C2,'+','?');
	}
}

// Lecture d'une donnée sur un bus I2C
int lisI2C(int canalI2C)
{
	int data=wiringPiI2CRead(canalI2C);
	if (debug) printw("readI2C %d,[%X]\n",canalI2C,data);
	return data;
}

// Lecture du registre d'état
void lisRegistreEtat(void)
{
	int i;
	ecrisI2C(canalI2C1, R_ETAT, NO_DATA);
	delay(pauseEcritureI2C);
	int data=lisI2C(canalI2C1);
	int bit;
    for (i=15; i>=0; i--)
	{
		bit=(((data>>i)&1)==1);
		switch (i)
		{
			case 0 : echo1=bit; break;
			case 1 : bouton1=bit; break;
			case 2 : bouton2=bit; break;
			case 3 : bouton3=bit; break;
			case 4 : bouton4=bit; break;
			case 5 : bouton5=bit; break;
		}
	}
}

int codeRegistre(int registre, int plage, int valeur)
{
    int maskEt;
    int maskOu;
    
	switch (plage)
	{
		case 1 : { maskOu=0x000F; maskEt=0xFFF0; break; }
		case 2 : { maskOu=0x00F0; maskEt=0xFF0F; break; }
		case 3 : { maskOu=0x0F00; maskEt=0xF0FF; break; }
		case 4 : { maskOu=0xF000; maskEt=0x0FFF; break; }
	}
	return ((registre&maskEt)|maskOu);
}

// Ecriture du registre d'état
void ecrisRegistreEtat(int registre)
{
	ecrisI2C(canalI2C1, R_COMD, registre);
}
