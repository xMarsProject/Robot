#include <iostream>
#include <stdio.h>
#include "driver.h"
#include "test.hpp"
#include <string.h>
#include <stdlib.h>

using namespace std;

struct data_s
{
    int v_ANG,v_LWH,v_RWH,v_LSO,v_RSO,v_FSO,v_LSN,v_RSN,v_LLI,v_RLI,v_LMO,v_RMO,v_HSM,v_VSM,vi_IRKEY;
    char *v_IRKEY;
    pthread_mutex_t mutex;
};

struct data_s data;

void *th_compass ( void *arg );

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

void do_bot(void)
{
    pthread_t camera;
    pthread_t compas;
    pthread_t sonar;
    pthread_t infra_red;

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
        cout<<"Error ... with the sonar"<<endl;

    int i,j;
    int lft_snd;
    int rgt_snd;
    int lft_lgt;
    int rgt_lgt;
    i=j=
    +0;
    data.v_HSM=data.v_VSM=0;
    while (1)
    {
        i++;
        j++;
        printf("NNNN ANG LWH RWH LSO RSO FSO LSN RSN LLI RLI LMO RMO HSM VSM IRKEY\n");
        bot_get_sensor(&lft_snd,&rgt_snd,&lft_lgt,&rgt_lgt);
        if (j>=1&j<=5) {bot_sm_left(); data.v_HSM=j; data.v_VSM=0;}
        if (j>=6&j<=10) {bot_sm_right(); data.v_HSM=j; data.v_VSM=0;}
        if (j>=11&j<=15) {bot_sm_up(); data.v_HSM=0; data.v_VSM=j;}
        if (j>=16&j<=20) {bot_sm_down(); data.v_HSM=0; data.v_VSM=j;}

        if (j>=1&j<=5) {bot_motor_left(); data.v_LMO=1;data.v_RMO=0;}
        if (j>=6&j<=10) {bot_motor_right(); data.v_LMO=0;data.v_RMO=1;}
        if (j>=11&j<=15) {bot_motor_advance(); data.v_LMO=1;data.v_RMO=1;}

        if (j>20) j=0;
        pthread_mutex_lock(&data.mutex);
        data.v_LSN=lft_snd;
        data.v_RSN=rgt_snd;
        data.v_LLI=lft_lgt;
        data.v_RLI=rgt_lgt;
        data.v_LWH=(int) bot_wheel_l;
        data.v_RWH=(int) bot_wheel_r;
        printf("%04d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %s\n",i,data.v_ANG,data.v_LWH,data.v_RWH,data.v_LSO,data.v_RSO,data.v_FSO,data.v_LSN,data.v_RSN,
               data.v_LLI,data.v_RLI,data.v_LMO,data.v_RMO,data.v_HSM,data.v_VSM,data.v_IRKEY);
        pthread_mutex_unlock(&data.mutex);
        if (data.vi_IRKEY==KEY_POWER) break;

        bot_wait(1000);
    }
    pthread_mutex_destroy(&data.mutex);
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



