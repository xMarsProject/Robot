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

void test_led()
{
    cout << "Press a key to continue"<<endl;
    cin.get();
    /*******************************************************
    * LED
    ********************************************************/
    cout << "Bot blinks 3 times"<<endl;
    bot_blink(3);
}


void test_button(void)
{
    /*******************************************************
    * BUTTON
    ********************************************************/
    cout << "Press button on the board to continue" << endl;
    bot_wait_button();
}

void test_sonar(void)
{
    /*******************************************************
    * SONAR
    ********************************************************/
    cout << "Sonar is giving the distance" << endl;
    while(1)
    {
        cout << bot_get_front_distance()<<" cm (press button to continue)"<<endl;
        bot_wait(250);
        if (bot_button_is_pushed()) break;
    }
    cout << "Press a key to continue" << endl;
    cin.get();
}

void test_ps_led(void)
{
    /*******************************************************
    * POWER SHIELD LED
    ********************************************************/
    cout << "On the power shield the TX LED blinks 10 times" << endl;
    bot_ps_blink(10);
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

void test_accel(void)
{
    /*******************************************************
        * POWER SHIELD ACCELEROMETER
        ********************************************************/
    cout << "Accelerometer is giving the pitch and roll" << endl;
    double pitch,roll;
    while(1)
    {
        bot_read_accel(&pitch,&roll);
        cout << " pitch"<<pitch<<"° roll="<<roll<<"(press button to continue)"<<endl;
        bot_wait(250);
        if (bot_button_is_pushed()) break;
    }
    cout << "Press a key to continue" << endl;
    cin.get();
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
    cout << "Press a key to continue" << endl;
    cin.get();
    /*******************************************************
    * Encoded wheels
    ********************************************************/

    cout << "Encoders give the traversed distance" << endl;
    while(1)
    {
        cout << "Left="<<bot_wheel_l<<"  cm, Right="<<bot_wheel_r<<" cm"<<endl;
        bot_wait(250);
        if (bot_button_is_pushed()) break;
    }
}

void *test_cam ( void *arg )
{
    (void) arg;
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


void test(void)
{
    cout << "Test program to check if software and hardware are ok" << endl;
    ///test_led();
    //test_button();
    //test_sonar();
    //test_ps_led();
    //test_motor();
    //test_accel();
    //test_servo();
    test_wheel();
    cout << "Press a key to continue" << endl;
    cin.get();
}
