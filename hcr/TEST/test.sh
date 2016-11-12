#!/bin/sh

# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version originale
#  Vincent PINEAU 10/11/2016
#  Script linux de compilation du pilote sur le Raspberry

cd /home/pi/HCR
rm ./test
gcc -c ./test.c -o ./test.o
gcc -o ./test ./test.o -ll -lm -lwiringPi -lncurses -lmenu -lrt
rm ./*.o
sudo ./test
