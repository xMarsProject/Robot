#!/usr/bin/python
# -*- coding: utf8 -*-
#
# Programme de test du robot
#

from Robot import *
import time
import curses

# Configuration des ports GPIO (26 au maximum) numérotation BCM

#Bouton poussoir principal
BUTTON1 = 20

#Bouton tilt
BUTTON2 = 16

#Les leds
RLED    = 21
GLED    = 18

#Sonar
TRIGGER = 26
ECHO    = 19

# Interface I2C 
SDA     = 02
SCL     = 03
I2CADR  = 0x53 #pour l'accéléromètre
I2CCTL  = 0x12 #pour la carte de puissance

# Programme principal

def main_prog(self,Bot):
    self.Ended=False
    Bot.RL.On()
    while self.running:
        time.sleep(1)
        s="%.1f cm    " % (Bot.SO.GetDistance())
        #print s
        axes = Bot.Ctl.getAxes(True)
        s1="x=%.0f°" % ( axes['x'] )
        s2="y=%.0f°" % ( axes['y'] )
        s3="z=%.0f°" % ( axes['z'] )
        #print s1,s2,s3
    Bot.RL.Off()
    self.Ended=True
    
# programmes secondaires

def sub1_prog(self,Bot):
    self.Ended=False
    while self.running:
        Bot.GL.On()
        Bot.Ctl.LedOn()
        time.sleep(1)
        Bot.GL.Off()
        Bot.Ctl.LedOff()
        time.sleep(1)
    self.Ended=True

def sub2_prog(self,Bot):
    self.Ended=False
    while self.running:
        time.sleep(0.2)
        if Bot.B2.On():
            print "ok"
    self.Ended=True
    
def sub3_prog(self,Bot):
    self.Ended=False
    anglev=88
    angleh=85
    screen=curses.initscr()
    #curses.noecho() 
    #curses.curs_set(0) 
    screen.keypad(1)
    while self.running:
        event = screen.getch() 
        if event == curses.KEY_LEFT :
            angleh-=10
            if angleh<5:
                angleh=5
            Bot.Ctl.SMh(angleh)
        if event == curses.KEY_UP :
            anglev+=10
            if anglev>170:
                anglev=170
            Bot.Ctl.SMv(anglev)
        if event == curses.KEY_RIGHT :
            angleh+=10
            if angleh>170:
                angleh=170
            Bot.Ctl.SMh(angleh)
        if event == curses.KEY_DOWN :
            anglev-=10
            if anglev<5:
                anglev=5
            Bot.Ctl.SMv(anglev)
    curses.endwin()
    self.Ended=True
    
# Initialisation et lancement des threads

if __name__ == '__main__':
    InitRobot()
    log(str(os.getppid()))
    #Say("Bonjour je suis un robot")
    #Say(time.strftime("%H heure %M et %S seconde", time.localtime()))
    #Say2("Bonjour je suis un robot")
    #Say2(time.strftime("%H heure %M et %S seconde", time.localtime()))
    # Configuration des composants
    Bot.B1    = ButtonType("Bouton 1",BUTTON1)
    Bot.B2    = ButtonType("Bouton 2",BUTTON2)
    Bot.RL    = LedType("LED Rouge",RLED)
    Bot.GL    = LedType("LED Verte",GLED)
    Bot.SO    = SonarType("Sonar avant",TRIGGER,ECHO)
    Bot.Ctl   = ControllerType("Ctl",I2CCTL,I2CADR)
    # Lancement des threads
    main = BotThreadType(target=main_prog,args = [Bot])
    sub1 = BotThreadType(target=sub1_prog,args = [Bot])
    sub2 = BotThreadType(target=sub2_prog,args = [Bot])
    sub3 = BotThreadType(target=sub3_prog,args = [Bot])
    main.start()
    sub1.start()
    sub2.start()
    sub3.start()
    while (Bot.B1.On()==False):
        time.sleep(0.1)
    # Arrêt des threads
    main.stop()
    sub1.stop()
    sub2.stop()
    sub3.stop()
    while (main.Ended & sub1.Ended & sub2.Ended & sub3.Ended) is not True:
        time.sleep(0.1)
    # Désactivation des composants
    time.sleep(0.2)
    Bot.Ctl.SMh(85)
    time.sleep(0.2)
    Bot.Ctl.SMv(88)
    Fin()