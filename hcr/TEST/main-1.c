/*
Vincent Pineau  04/11/2016 Programme de pilotage du robot
*/

#include <stdio.h>
#include <time.h>
#include <curses.h>
#include "driver.h"
#include "main.h"

#define programmeVersion "V1 (04-11-2016 - Vincent PINEAU)"

clock_t t1, t2;
 
int main(int argc, char *argv[])
{
	
	if (init()==1) // Initialisation des cartes et protocoles
	{
	printw("Version du programme de test %s\n",programmeVersion);
		printw("Initialisation OK\n");
		t1 = clock();
		loop(); // Lancement programme de test
	}
	bot_close();
	printw("Fin du programme de test\n");
}

void loop()
{
  int fin=0;
  int ch;
  int registre;
  printw("Début du programme de test\n");
  for (;;) 
  {
    if ((ch = getch()) == ERR) 
    {
      t2 = clock();   
      float diff = ((float)(t2 - t1) / 1000000.0F ) * 1000;
      if (diff>100)
      {	
    	t1 = clock();
		lisRegistreEtat();
		if (bouton1) printw("Vous avez appuyé sur le bouton 1\n");
		if (bouton2) printw("Vous avez appuyé sur le bouton 2\n");
		if (bouton3) printw("Vous avez appuyé sur le bouton 3\n");
		if (bouton4) printw("Vous avez appuyé sur le bouton 4\n");
		if (bouton5) printw("Vous avez appuyé sur le bouton 5\n");
	  }
    }
    else 
    {
    	switch (ch)
        {
           case KEY_UP        : printw("<\n"); break;
           case KEY_DOWN      : printw("<\n"); break;
           case KEY_LEFT      : printw("<\n"); break;
           case KEY_RIGHT     : printw("<\n"); break;
           case KEY_BACKSPACE : fin=1; break;
        }
    }
    if (fin) break;
  }
}