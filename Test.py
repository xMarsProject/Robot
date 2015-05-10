#!/usr/bin/python
# -*- coding: utf8 -*-
import time
import threading


class ComponentType:
    nbCmp=0 
    def __init__(self, nom):
        self.nom=nom
        ComponentType.nbCmp+=1
        self.ident=ComponentType.nbCmp

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

class ControllerType(ComponentType):

    def __init__(self, nom, address = 0x12):
        ComponentType.__init__(self,nom)
        self.address = address
        self.lock = threading.Lock()
        self.cmd_go     = 0
        self.cmd_ok     = 1
        self.cmd_LedOn  = 2
        self.cmd_LedOff = 3
        self.cmd_SMh    = 4
        self.cmd_SMv    = 5
    
    def LedOn(self,qui):
        self.lock.acquire()
        try:
            for i in range(1,100):
                print qui+" "+str(i)
                time.sleep(0.1)
        finally:
            #pass
            self.lock.release()
            
    def LedOff(self,qui):
        self.lock.acquire()
        try:
            for i in range(1,100):
                time.sleep(0.1)
                print qui+" "+str(i)
            
        finally:
            #pass
            self.lock.release()
            

def sub1_prog(self,Bot):
    self.Ended=False
    while self.running:
        #print "tente th1"
        Bot.Ctl.LedOn("th1")
        #print "th1 fini"
    self.Ended=True

def sub2_prog(self,Bot):
    self.Ended=False
    while self.running:
        #print "te"
        Bot.Ctl.LedOff("th2")
        #print "th2 fini"
    self.Ended=True
 
Bot=ComponentType("Robot")
Bot.Ctl   = ControllerType("Ctl",12)
sub1 = BotThreadType(target=sub1_prog,args = [Bot])
sub2 = BotThreadType(target=sub2_prog,args = [Bot])
sub1.start()
sub2.start()
time.sleep(10)
sub1.stop()
sub2.stop()
