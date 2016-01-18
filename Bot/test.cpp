#include "test.hpp"
#include "driver.h"
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

/*
pthread_t th1;

    if(pthread_create(&th1,NULL, test_cam,NULL )==-1)
    {
        cout<<"Error ... with the camera"<<endl;
        return EXIT_FAILURE;
    }*/
using namespace std;


void test_sonar(void)
{
    /*******************************************************
    * SONAR
    ********************************************************/
    cout << "Sonar is giving the distance" << endl;
    while(1)
    {
       cout << bot_get_left_distance()<<" cm (left)"<<endl;
       cout << bot_get_front_distance()<<" cm (front)"<<endl;
       cout << bot_get_right_distance()<<" cm (right)"<<endl;
       bot_wait(250);
        //if (bot_button_is_pushed()) break;
    }
    cout << "Press a key to continue" << endl;
    cin.get();
}

void test_servo(void)
{

    /*******************************************************
    * POWER SHIELD SERVO MOTOR
    ********************************************************/
    cout << "Use keybord direction keys to move the servo motor, DEL key ends the test" << endl;
    int c;
    while(1)
    {
        c=cin.get();
        if (c==27)
        {
            c=cin.get();
            if (c==91)
            {
                c=cin.get();
                switch(c)
                {
                case 68 :
                    bot_sm_left();
                    break;// left
                case 67 :
                    bot_sm_right();
                    break;// right
                case 65 :
                    bot_sm_up();
                    break;// up
                case 66 :
                    bot_sm_down();
                    break;// down
                }
            }
        }
        if (c==127) break;
    }
}

void test_motor(void)
{
    /*******************************************************
    * POWER SHIELD SERVO MOTOR
    ********************************************************/
    int c;
    cout << "Use keybord direction keys to move the robot, DEL key ends the test and space bar stops the robot" << endl;
    while(1)
    {
        c=cin.get();
        if (c==32) bot_motor_stop();
        if (c==27)
        {
            c=cin.get();
            if (c==91)
            {
                c=cin.get();
                switch(c)
                {
                case 68 :
                    bot_motor_left();
                    break;// left
                case 67 :
                    bot_motor_right();
                    break;// right
                case 65 :
                    bot_motor_advance();
                    break;// up
                case 66 :
                    bot_motor_back();
                    break;// down
                }
            }
        }
        if (c==127) break;
    }
    bot_motor_stop();
}

void test_wheel(void)
{
    //cout << "Press a key to continue" << endl;
    //cin.get();
    /*******************************************************
    * Encoded wheels
    ********************************************************/

    cout << "Encoders give the traversed distance" << endl;
    while(1)
    {
        cout << "Left="<<bot_wheel_l<<"  cm, Right="<<bot_wheel_r<<" cm"<<endl;
        bot_wait(250);
        //if (bot_button_is_pushed()) break;
    }
}

void test_cam ( )
{
    cout<<"Opening Camera..."<<endl;
    cv::namedWindow("Video",CV_WINDOW_AUTOSIZE);
    raspicam::RaspiCam_Cv Camera;
    cv::Mat image1;
    cv::Mat image2;

    //Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    Camera.set ( CV_CAP_PROP_FRAME_WIDTH, 320);
    Camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 240 );

    if (!Camera.open())
    {
        cerr<<"Error opening the camera"<<endl;
    }
    while (1)
    {
        int H=image1.rows/2;
        int W=image1.cols/2;
        Camera.grab();
        Camera.retrieve ( image1);
        cv::Mat matRotation = cv::getRotationMatrix2D(cv::Point(W,H),180,1);
        warpAffine(image1,image2,matRotation,image1.size());
        cv::imshow("Video",image2);
        if  (cv::waitKey(1)==27) break;
    }
    cout<<"Stop camera..."<<endl;
    Camera.release();
    cv::destroyWindow("Video");
    pthread_exit(NULL);
}

void test_sensor()
{
    while (1)
    {
        int lft_snd;
        int rgt_snd;
        int lft_lgt;
        int rgt_lgt;
        bot_get_sensor(&lft_snd,&rgt_snd,&lft_lgt,&rgt_lgt);
        cout << "Left snd="<<lft_snd<<",Right snd="<<rgt_snd<<",Left lgt="<<lft_lgt<<",Right lgt="<<rgt_lgt<<endl;
        bot_wait(10);
    }
}

void test_IR(void)
{
    bot_wait_IR_cmd(KEY_POWER);
    cout << "Power key pressed"<<endl;
    cout << "key pressed :"<<bot_wait_IR()<<endl;
}

void test_compass(void)
{
    float angle;
    short x,y,z;
    while (1)
    {
        angle=bot_get_compass_xyz(&x,&y,&z);
        //cout << "angle="<<angle<<" "<< x << y << x << endl;
        bot_wait(250);
    }
}

void test(void)
{
    cout << "Test program to check if software and hardware are ok" << endl;
    //test_cam();
    //test_led();
    //test_button();
    //test_sonar();
    //test_ps_led();
    //test_motor();
    //test_accel();
    //test_servo();
    //test_wheel();
    //test_sensor();
    //test_IR();
    test_compass();
    cout << "Press a key to continue" << endl;
    cin.get();
}


