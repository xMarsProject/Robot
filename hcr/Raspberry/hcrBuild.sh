#!/bin/sh

# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version originale
#  Vincent PINEAU 10/11/2016
#  Script linux de compilation du pilote sur le Raspberry

cd /home/pi/HCR
rm ./hcrTest
gcc -c ./hcrMain.c -o ./hcrMain.o
gcc -c ./hcrDriver.c -o ./hcrDriver.o
gcc -o ./hcrTest ./hcrMain.o ./hcrDriver.o -ll -lm -lwiringPi -lncurses -lrt
rm ./*.o
sudo ./hcrTest
