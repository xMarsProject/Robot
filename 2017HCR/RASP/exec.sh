#!/bin/sh

# Projet HCR (Home Car Robot)
# Plateforme robotique open source DFRobot (www.dfrobot.com)

#  Version originale
#  Vincent PINEAU 10/11/2016
#  Script linux de compilation du pilote sur le Raspberry

cd /home/pi/2017HCR
sudo rm ./hcrTest
gcc -c ./main.c -o ./main.o
gcc -c ./pilote.c -o ./pilote.o
gcc -o ./hcrTest ./pilote.o ./main.o -ll -lm -lwiringPi -lncurses -lrt -lpthread
rm ./*.o
sudo ./hcrTest
