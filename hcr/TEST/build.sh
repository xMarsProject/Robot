#!/bin/sh
cd /home/pi/Bot2017
rm ./robot
gcc -c ./main.c -o ./main.o
gcc -c ./driver.c -o ./driver.o
gcc -o ./robot ./main.o ./driver.o -ll -lm -lwiringPi -lncurses
rm ./*.o
sudo ./robot 
