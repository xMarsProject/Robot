#!/usr/bin/python
# -*- coding: utf8 -*-
#
# Programme de test du robot
#

from Robot import *
import time

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

# Interface I2C pour l'accéléromètre
SDA     = 02
SCL     = 03
I2CADR  = 0x53

class Controller(object):
    def __init__(self):
        self.lock = threading.Lock()
        
    def Operation(self,Op):
        self.lock.acquire()
        try:
            time.sleep(5)
        finally:
            self.lock.release()

# Programme principal

def main_prog(self,Bot):
    self.Ended=False
    Bot.RL.On()
    while self.running:
        time.sleep(1)
        s="%.1f cm    " % (Bot.SO.GetDistance())
        print s
        axes = Bot.Accel.getAxes(True)
        s1="x=%.0f°" % ( axes['x'] )
        s2="y=%.0f°" % ( axes['y'] )
        s3="z=%.0f°" % ( axes['z'] )
        print s1,s2,s3
    Bot.RL.Off()
    self.Ended=True
    
# programmes secondaires

def sub_prog(self,Bot):
    self.Ended=False
    while self.running:
        Bot.GL.On()
        time.sleep(0.2)
        Bot.GL.Off()
        time.sleep(0.2)
    self.Ended=True

def sub2_prog(self,Bot):
    self.Ended=False
    while self.running:
        time.sleep(0.2)
        if Bot.B2.On():
            print "ok"
    self.Ended=True
    
# Initialisation et lancement des threads

if __name__ == '__main__':
    InitRobot()
    Say("Bonjour je suis un robot")
    Say(time.strftime("%H heure %M et %S seconde", time.localtime()))
    Say2("Bonjour je suis un robot")
    Say2(time.strftime("%H heure %M et %S seconde", time.localtime()))
    # Configuration des composants
    Bot.B1    = ButtonType("Bouton 1",BUTTON1)
    Bot.B2    = ButtonType("Bouton 2",BUTTON2)
    Bot.RL    = LedType("LED Rouge",RLED)
    Bot.GL    = LedType("LED Verte",GLED)
    Bot.SO    = SonarType("Sonar avant",TRIGGER,ECHO)
    Bot.Accel = AccelerometerType("Accel",I2CADR)
    # Lancement des threads
    main = BotThreadType(target=main_prog,args = [Bot])
    sub = BotThreadType(target=sub_prog,args = [Bot])
    sub2 = BotThreadType(target=sub2_prog,args = [Bot])
    
    main.start()
    sub.start()
    sub2.start()
    
    while (Bot.B1.On()==False):
        time.sleep(0.1)
    # Arrêt des threads
    main.stop()
    sub.stop()
    sub2.stop()
    while (main.Ended & sub.Ended & sub2.Ended) is not True:
        time.sleep(0.1)
    # Désactivation des composants
    Fin()