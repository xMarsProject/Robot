#include <iostream>
#include <stdio.h>
#include "driver.h"
#include "test.hpp"
#include <string.h>
#include <stdlib.h>

using namespace std;
void disp(void);
struct data_s
{
    int v_ANG,v_LWH,v_RWH,v_LSO,v_RSO,v_FSO,v_LSN,v_RSN,v_LLI,v_RLI,v_LMO,v_RMO,v_HSM,v_VSM,vi_IRKEY;
    char *v_IRKEY;
    pthread_mutex_t mutex;
};

struct data_s data;

void *th_compass (  void *arg )
{
    short x,y,z;
    while(1)
    {
        bot_wait(100);
        pthread_mutex_lock(&data.mutex);
        data.v_ANG=(int)bot_get_compass_xyz(&x,&y,&z);
        pthread_mutex_unlock(&data.mutex);
    }
}

void *th_sonar (  void *arg )
{
    int l,r,f;
    while(1)
    {
        bot_wait(30);
        l=bot_get_left_distance();
        bot_wait(30);
        r=bot_get_right_distance();
        bot_wait(30);
        f=bot_get_front_distance();
        pthread_mutex_lock(&data.mutex);
        data.v_LSO=l;
        data.v_RSO=r;
        data.v_FSO=f;
        pthread_mutex_unlock(&data.mutex);
    }
}

void *th_IR (  void *arg )
{
    char *code;
    while(1)
    {
        code=bot_get_ir_ccode(bot_wait_IR_c());
        pthread_mutex_lock(&data.mutex);
        data.v_IRKEY=code;
        data.vi_IRKEY=bot_get_ir_code(code);
        pthread_mutex_unlock(&data.mutex);
    }
}

void *th_LS (  void *arg )
{
    int lft_snd;
    int rgt_snd;
    int lft_lgt;
    int rgt_lgt;

    while (1)
    {
        pthread_mutex_lock(&data.mutex);
        //bot_get_sensor(&lft_snd,&rgt_snd,&lft_lgt,&rgt_lgt);
        data.v_LSN=lft_snd;
        data.v_RSN=rgt_snd;
        data.v_LLI=lft_lgt;
        data.v_RLI=rgt_lgt;
        pthread_mutex_unlock(&data.mutex);
        bot_wait(500);
    }
}

void do_bot(void)
{
    int lft_snd;
    int rgt_snd;
    int lft_lgt;
    int rgt_lgt;
    float angle;
    short x,y,z;
    unsigned char Speed=50;
    int c;
    bool test_key=true;
    pthread_t camera;
    if(pthread_create(&camera,NULL, test_cam,NULL )==-1)
        cout<<"Error ... with the camera"<<endl;
    while(1)
    {
        c=cin.get();
        if (test_key) cout << c << endl;
        switch (c)
        {
        case 105 : // i infra rouge
            bot_motor_stop();
            cout << "Attente de la télécommande"<<endl;
            cout << bot_get_ir_ccode(bot_wait_IR_c())<<endl;
            cout << "Fin du test"<<endl;
            break;
        case 43 : // + vitesse ++
            Speed+=5;
            bot_motor_speed(Speed);
            cout << "Vitesse "<<(int)Speed<<endl;
            break;
        case 45 : // + vitesse --
            Speed-=5;
            bot_motor_speed(Speed);
            cout << "Vitesse "<<(int)Speed<<endl;
            break;
        case 98 : //b boussole
            angle=bot_get_compass_xyz(&x,&y,&z);
            cout << "angle="<<angle<< endl;
            break;
        case 99 : //c capteurs
            bot_get_sensor(&lft_snd,&rgt_snd,&lft_lgt,&rgt_lgt);
            cout << "Left snd="<<lft_snd<<",Right snd="<<rgt_snd<<",Left lgt="<<lft_lgt<<",Right lgt="<<rgt_lgt<<endl;
            break;
        case 100 : //d distance
            cout << "Left="<<bot_wheel_l<<"  cm, Right="<<bot_wheel_r<<" cm"<<endl;
            break;
        case 63 : // ? help
            cout << "******************* Commandes *********************"<<endl;
            cout << "s            : distance donnée par les sonar" << endl;
            cout << "flèche       : dirige le robot"<<endl;
            cout << "espace       : stop le robot"<<endl;
            cout << "shift+flèche : déplace la caméra"<<endl;
            cout << "c            : affiche les capteurs son et lumière"<<endl;
            cout << "q            : Quitte le programme"<<endl;
            cout << "d            : affiche la distance parcourue"<<endl;
            cout << "b            : affiche la boussole"<<endl;
            cout << "i            : test télécommande infra rouge"<<endl;
            cout << "+            : Augmente la vitesse"<<endl;
            cout << "-            : Diminue la vitesse"<<endl;
            cout << "***************************************************"<<endl;
            break;
        case 115 : //S : Sonar
            cout << bot_get_left_distance()<<" cm (left)"<<endl;
            cout << bot_get_front_distance()<<" cm (front)"<<endl;
            cout << bot_get_right_distance()<<" cm (right)"<<endl;
            break;
        case 32 :
            bot_motor_stop();
            break;
        case 27 :
            c=cin.get();
            if (test_key) cout << c << endl;
            if (c==91)
            {
                c=cin.get();
                if (test_key) cout << c << endl;
                switch(c)
                {
                case 68 :
                    bot_motor_stop();
                    bot_motor_left();
                    break;// left
                case 67 :
                    bot_motor_stop();
                    bot_motor_right();
                    break;// right
                case 65 :
                    bot_motor_stop();
                    bot_motor_advance();
                    break;// up
                case 66 :
                    bot_motor_stop();
                    bot_motor_back();
                    break;// down
                case 49 :
                    c=cin.get();
                    if (test_key) cout << c << endl;
                    if  (c==59)
                    {
                        c=cin.get();
                        if (test_key) cout << c << endl;
                        if (c==50)
                        {
                            c=cin.get();
                            if (test_key) cout << c << endl;
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
                }
            }
        }
        if (c==113) break; //q
    }
    bot_motor_stop();
}

void do_bot2(void)
{
    pthread_t camera;
    pthread_t compas;
    pthread_t sonar;
    pthread_t infra_red;
    pthread_t light_sound;

    data.v_IRKEY=(char*)malloc(sizeof(char)*255);
    strcpy(data.v_IRKEY,"XXXXX");
    data.v_ANG=data.v_LWH=data.v_RWH=data.v_LSO=data.v_RSO=data.v_FSO=data.v_LSN=data.v_RSN=data.v_LLI=data.v_RLI=data.v_LMO=data.v_RMO=data.v_HSM=data.v_VSM=1;
    data.mutex=PTHREAD_MUTEX_INITIALIZER;

    if(pthread_create(&camera,NULL, test_cam,NULL )==-1)
        cout<<"Error ... with the camera"<<endl;

    if(pthread_create(&compas,NULL, th_compass,NULL )==-1)
        cout<<"Error ... with the compass"<<endl;

    if(pthread_create(&sonar,NULL, th_sonar,NULL )==-1)
        cout<<"Error ... with the sonar"<<endl;

    if(pthread_create(&infra_red,NULL, th_IR,NULL )==-1)
        cout<<"Error ... with the infra red"<<endl;

    if(pthread_create(&light_sound,NULL, th_LS,NULL )==-1)
        cout<<"Error ... with the light & sound"<<endl;

    bool end=false;
    while (!end)
    {
        pthread_mutex_lock(&data.mutex);
        data.v_LWH=(int) bot_wheel_l;
        data.v_RWH=(int) bot_wheel_r;
        //disp();
        switch (data.vi_IRKEY)
        {
        case KEY_0 :
            disp();
            break;
        case KEY_1 :
            disp();
            break;
        case KEY_2 :
            disp();
            bot_sm_up();
            break;
        case KEY_3 :
            disp();
            break;
        case KEY_4 :
            disp();
            bot_sm_left();
            break;
        case KEY_5 :
            disp();
            break;
        case KEY_6 :
            disp();
            bot_sm_right();
            break;
        case KEY_7 :
            disp();
            break;
        case KEY_8 :
            disp();
            bot_sm_down();
            break;
        case KEY_9 :
            disp();
            break;
        case KEY_POWER :
            disp();
            end=true;
            break;
        case KEY_VOLUMEUP :
            disp();
            bot_motor_advance();
            //bot_wait(5000);
            ///bot_motor_stop();
            break;
        case KEY_VOLUMEDOWN :
            disp();
            bot_motor_back();
            break;
        case KEY_UP :
            disp();
            break;
        case KEY_DOWN :
            disp();
            break;
        case KEY_REWIND :
            disp();
            bot_motor_left();
            break;
        case BTN_FORWARD :
            disp();
            bot_motor_right();
            break;
        case KEY_PLAYPAUSE :
            disp();
            bot_motor_stop();
            break;
        case KEY_F1 :
            disp();
            break;
        case KEY_F2 :
            disp();
            break;
        case KEY_F3 :
            disp();
            break;
        }
        //disp();
        data.vi_IRKEY=99;
        pthread_mutex_unlock(&data.mutex);
        bot_wait(10);
    }
    pthread_mutex_destroy(&data.mutex);
}

void disp(void)
{
    printf("ANG LWH RWH LSO RSO FSO LSN RSN LLI RLI IRKEY\n");
    printf("%03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %s\n",data.v_ANG,data.v_LWH,data.v_RWH,data.v_LSO,data.v_RSO,data.v_FSO,data.v_LSN,data.v_RSN,
           data.v_LLI,data.v_RLI,data.v_IRKEY);
}

int main()
{
    cout<<"Bot V3"<<endl;
    if (!bot_init())
    {
        cout<<"Error ... End of program"<<endl;
        return 0;
    }

    //test();
    do_bot();
    bot_close();
    cout<<"End of program"<<endl;
    return 0;
}



