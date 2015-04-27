#!/usr/bin/python
# -*- coding: utf8 -*-
#
# Ensemble des classes pour piloter le Robot
#

import threading
import time
import math
import smbus
import RPi.GPIO as GPIO
import os
import urllib

# select the correct i2c bus for this revision of Raspberry Pi
revision = ([l[12:-1] for l in open('/proc/cpuinfo','r').readlines() if l[:8]=="Revision"]+['0000'])[0]
bus = smbus.SMBus(1 if int(revision, 16) >= 4 else 0)

# ADXL345 constants
EARTH_GRAVITY_MS2   = 9.80665
SCALE_MULTIPLIER    = 0.004

DATA_FORMAT         = 0x31
BW_RATE             = 0x2C
POWER_CTL           = 0x2D

BW_RATE_1600HZ      = 0x0F
BW_RATE_800HZ       = 0x0E
BW_RATE_400HZ       = 0x0D
BW_RATE_200HZ       = 0x0C
BW_RATE_100HZ       = 0x0B
BW_RATE_50HZ        = 0x0A
BW_RATE_25HZ        = 0x09

RANGE_2G            = 0x00
RANGE_4G            = 0x01
RANGE_8G            = 0x02
RANGE_16G           = 0x03

MEASURE             = 0x08
AXES_DATA           = 0x32

def InitRobot():
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    
def Fin():
    GPIO.cleanup()



#############################################################
#
# Base Class
#
#############################################################

class ComponentType:
    nbCmp=0 
    def __init__(self, nom):
        self.nom=nom
        ComponentType.nbCmp+=1
        self.ident=ComponentType.nbCmp

Bot=ComponentType("Robot")

class BotThreadType(threading.Thread):
    def __init__(self,target, args= [], kwargs={}):
        threading.Thread.__init__(self)
        self.running = False
        self._target = target 
        self._args = args 
        self._kwargs = kwargs
 
    def run(self):
        self.running = True
        self._target(self,*self._args, **self._kwargs) 
 
    def stop(self):
        self.running = False

#############################################################
#
# Button
#
#############################################################

class ButtonType(ComponentType):
    def __init__(self, nom,pin):
        ComponentType.__init__(self,nom)
        self.pin=pin
        GPIO.setup(pin, GPIO.IN)
        
    def On(self):
        if GPIO.input(self.pin)==0:
            return True
        else:
            return False
    
    def Off(self):
        if GPIO.input(self.pin)!=0:
            return True
        else:
            return False
    
#############################################################
#
# LED
#
#############################################################

class LedType(ComponentType):
    def __init__(self, nom,pin):
        ComponentType.__init__(self,nom)
        self.pin=pin
        GPIO.setup(pin, GPIO.OUT)
    
    def On(self):
        GPIO.output(self.pin, GPIO.LOW)
    
    def Off(self):
        GPIO.output(self.pin, GPIO.HIGH)

#############################################################
#
# Sonar
#
#############################################################

class SonarType(ComponentType):
    def __init__(self, nom,trigger,echo):
        ComponentType.__init__(self,nom)
        self.trigger=trigger
        self.echo=echo
        self.distance=0
        self.trigger=trigger
        self.echo=echo
        GPIO.setup(trigger,GPIO.OUT)  
        GPIO.setup(echo,GPIO.IN)
        GPIO.output(self.trigger, False)
        
    def GetDistance(self):
        GPIO.output(self.trigger, True)
        time.sleep(0.00001)
        GPIO.output(self.trigger, False)
        while GPIO.input(self.echo)==0:
            start = time.time()
        while GPIO.input(self.echo)==1:
            stop = time.time()
        # Calcul du temps écoulé
        elapsed = stop-start
        distance = elapsed * 34300 # Multiplié par la vitesse du son en cm/sec       
        distance = distance / 2 # divisé par deux car aller/retour
        self.distance=distance
        return distance
    

#############################################################
#
# Accelerometer
#
#############################################################        
        
class AccelerometerType(ComponentType):

    address = None

    def __init__(self, nom, address = 0x53):
        ComponentType.__init__(self,nom)
        self.address = address
        self.setBandwidthRate(BW_RATE_100HZ)
        self.setRange(RANGE_2G)
        self.enableMeasurement()

    def enableMeasurement(self):
        bus.write_byte_data(self.address, POWER_CTL, MEASURE)

    def setBandwidthRate(self, rate_flag):
        bus.write_byte_data(self.address, BW_RATE, rate_flag)

    # set the measurement range for 10-bit readings
    def setRange(self, range_flag):
        value = bus.read_byte_data(self.address, DATA_FORMAT)

        value &= ~0x0F;
        value |= range_flag;  
        value |= 0x08;

        bus.write_byte_data(self.address, DATA_FORMAT, value)
    
    # returns the current reading from the sensor for each axis
    #
    # parameter gforce:
    #    False (default): result is returned in m/s^2
    #    True           : result is returned in gs
    def getAxes(self, gforce = False):
        bytes = bus.read_i2c_block_data(self.address, AXES_DATA, 6)
        
        x = bytes[0] | (bytes[1] << 8)
        if(x & (1 << 16 - 1)):
            x = x - (1<<16)

        y = bytes[2] | (bytes[3] << 8)
        if(y & (1 << 16 - 1)):
            y = y - (1<<16)

        z = bytes[4] | (bytes[5] << 8)
        if(z & (1 << 16 - 1)):
            z = z - (1<<16)

        x = x * SCALE_MULTIPLIER 
        y = y * SCALE_MULTIPLIER
        z = z * SCALE_MULTIPLIER

        if gforce == False:
            x = x * EARTH_GRAVITY_MS2
            y = y * EARTH_GRAVITY_MS2
            z = z * EARTH_GRAVITY_MS2

        x = round(x, 4)
        y = round(y, 4)
        z = round(z, 4)
        total = math.sqrt(x*x+y*y+z*z)
        x = round(math.asin(x/ total )*180.0/3.1416);
        y = round(math.asin(y/ total )*180.0/3.1416);
        z = round(math.acos(z/ total )*180.0/3.1416);
        return {"x": x, "y": y, "z": z}

def Say2(txt):
    fichier = open("exemple.txt", "w")
    fichier.write(time.strftime("%H heure %M et %S seconde", time.localtime()))
    fichier.close()
    os.system("espeak -v mb/mb-fr1 -f exemple.txt | mbrola /usr/share/mbrola/voices/fr1 - - | aplay -r16000 -fS16")


def Say(txt):
    os.system('mpg321 "http://translate.google.com/translate_tts?tl=fr&q='+urllib.quote_plus(txt)+'"')