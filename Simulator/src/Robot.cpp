#include <iostream>
#include <windows.h>
#include <time.h>
#include "Robot.h"

#define DETECT L=bot_get_left_distance(); F=bot_get_front_distance(); R=bot_get_right_distance();
#define ROUND 0
#define MAXDIST 9999
#define MINDIST 10

using namespace std;

void Robot::Run()
{
    Stop=false;
    Sleep(1000);
    do_bot();
    Stop=true;
}

void Robot::do_bot()
{

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunRight(90,ROUND);

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunRight(90,ROUND);

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunRight(90,ROUND);

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunRight(90,ROUND);

bot_motor_advance();
    bot_wait(1000);


bot_motor_advance();
    bot_wait(1000);
    Bot_TrunLeft(90,ROUND);

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunLeft(90,ROUND);

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunLeft(90,ROUND);

    bot_motor_advance();
    bot_wait(1000);
    Bot_TrunLeft(90,ROUND);
/*
    int L,F,R;
    int tazimuth;
    bot_init();
    bot_motor_advance();
    int counter=0;
    while ( true )
    {
        DETECT;
        if (F<=MINDIST)
        {
            Bot_TrunRight(90,ROUND);
            counter--;
            FollowWall(0);
            DETECT;
            if (azimuth<90)
            {
                Bot_TrunRight(90-azimuth,ROUND);
            }
            else
            {
                if (azimuth<180)
                {
                    Bot_TrunRight(180-azimuth,ROUND);
                }
                else
                {
                    if (azimuth<270)
                    {
                        Bot_TrunRight(270-azimuth,ROUND);
                    }
                    else
                    {
                        if (azimuth<360)
                        {
                            Bot_TrunRight(360-azimuth,ROUND);
                        }
                    }
                }
            }
        }
            DETECT;
            if (F<=MINDIST)
            {
                Bot_TrunRight(90,ROUND);
                FollowWall(0);
            }
            //cout << F << endl;
            //break;


        bot_wait(2);
    }
*/
    bot_close();
}

void Robot::FollowWall(int dir)
{
    int L,F,R;
    int correct=10;
    bool stp=false;
    if (dir==0) // Left
    {

        bot_motor_advance();
        DETECT;
        do
        {
            bot_wait(60);

            if (L>correct)
            {
                Bot_TrunLeft(1,0);
                bot_motor_advance();
            }
            else
            {
                Bot_TrunRight(1,0);
                bot_motor_advance();
            }
            DETECT;
        }
        while (F>MINDIST);
    }
    else // Right
    {

    }
}

void Robot::bot_reach_angle(int angle, int round)
{
    int tazimuth=azimuth;
    while (abs(azimuth-tazimuth)>(angle+round) || abs(azimuth-tazimuth)<angle-round )
    {
        bot_wait(10);
    }
}

void Robot::Bot_TrunLeft(int angle, int round)
{
    Bot_Stop();
    bot_motor_left();
    int start_angle=azimuth;
    int end_angle=start_angle;

    for (int i=start_angle; i<(start_angle+angle); i++)
    {
            if ((end_angle-1)<0)
                end_angle=360;
            else end_angle--;
    }

    do
    {
        bot_wait(2);
        bot_motor_left();
    } while (!in(azimuth,end_angle-5, end_angle+5));
}

bool Robot::in(int a,  int b, int c)
{
    return (a>=b && a<=c);
}

void Robot::Bot_TrunRight(int angle, int round)
{
    Bot_Stop();
    bot_motor_right();
    int start_angle=azimuth;
    int end_angle=start_angle;

    for (int i=start_angle; i<(start_angle+angle); i++)
    {
            if ((end_angle+1)>359)
                end_angle=0;
            else end_angle++;
    }

    do
    {
        ///bot_wait(2);
        bot_motor_right();
    } while (!in(azimuth,end_angle-5, end_angle+5));
}

void Robot::Bot_Stop()
{
    float distance;
    do
    {
        distance=bot_wheel_l;
        bot_motor_stop();
        bot_wait(2);
    }
    while (distance!=bot_wheel_l);
}

void Robot::bot_close()
{
    bot_motor_stop();
}

bool Robot::bot_init()
{
    bot_motor_stop();
    srand(time(NULL));
    ScanFront=9999;
    left=false;
    right=false;
    bback=false;
    advance=false;
    stop=true;
    return true;
}

void Robot::bot_wait(int t)
{
    Sleep(t);
}

int Robot::bot_get_front_distance()
{
    return ScanFront;
}

int Robot::bot_get_right_distance()
{
    return ScanRight;
}

int Robot::bot_get_left_distance()
{
    return ScanLeft;
}

void Robot::bot_motor_advance()
{
    stop=false;
    advance=true;
    bback=false;
    right=false;
    left=false;
}

void  Robot::bot_motor_back()
{
    advance=false;
    stop=false;
    bback=true;
    right=false;
    left=false;
}

void Robot::bot_motor_stop()
{
    advance=false;
    bback=false;
    stop=true;
    right=false;
    left=false;
}

void Robot::bot_motor_left()
{
    //std::cout<<"L"<<std::endl;
    left=true;
}

void Robot::bot_motor_right()
{
    //std::cout<<"R"<<std::endl;
    right=true;
}

Robot::Robot()
{
    //ctor
}

Robot::~Robot()
{
    //dtor
}
