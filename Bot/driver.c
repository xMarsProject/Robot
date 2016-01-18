/************** Driver Raspberry for Robot ***********************
* Vincent PINEAU 01/01/2016
* based on wiringPi Libary (wiringpi.com)
******************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <lirc/lirc_client.h>


#include "driver.h"

/* initialize the robot, all the components are  initialised even if you won't use them */
unsigned long int bot_encoder_l; // tick counter for left wheel
unsigned long int bot_encoder_r; // tock counter for right wheel
float bot_wheel_l; // number of cm for the lef wheel
float bot_wheel_r;// number of cm for the right wheel
struct lirc_config *config;
struct termios bot_keyboard;
int compass; // I2C compass (file description)

int bot_init()
{
    int bot_ps; // SPI channel to communicate with the power shield
    if (bot_cnf_terminal(&bot_keyboard)==-1) // configure the terminal to use the keybord key by key
    {
        printf("Keyboard can't be initialized ...\n");
        return 0;
    }

    wiringPiSetup();
    // sonar
    pinMode(bot_front_sonar_trigger,OUTPUT);
    pinMode(bot_front_sonar_echo,INPUT);
    digitalWrite(bot_front_sonar_trigger,HIGH);

    pinMode(bot_left_sonar_trigger,OUTPUT);
    pinMode(bot_left_sonar_echo,INPUT);
    digitalWrite(bot_left_sonar_trigger,HIGH);

    pinMode(bot_right_sonar_trigger,OUTPUT);
    pinMode(bot_right_sonar_echo,INPUT);
    digitalWrite(bot_right_sonar_trigger,HIGH);
    // Power shield
    pinMode(bot_spi_cmd, OUTPUT);
    bot_ps = wiringPiSPISetup (0,1000000);
    if (bot_ps==-1)
    {
        printf("Power shield can't be initialised\n");
        return 0;
    }

    bot_motor_stop();
    // encoded wheels
    pinMode(bot_left_encoded_wheel, INPUT);
    pinMode(bot_right_encoded_wheel, INPUT);
    bot_encoder_l=0;
    bot_encoder_r=0;
    bot_wheel_l=0;
    bot_wheel_r=0;
    wiringPiISR(bot_left_encoded_wheel,INT_EDGE_RISING,&interupt_LW); // set interupt to count  the ticks of the left wheel
    wiringPiISR(bot_right_encoded_wheel,INT_EDGE_RISING,&interupt_RW); // set interupt to count the ticks of the right wheel

    //Initiate LIRC. Exit on failure
    if(lirc_init("lirc",1)==-1)
    {
        printf("Infrared can't be initialised\n");
        return 0;
    }
    if(lirc_readconfig(NULL,&config,NULL)!=0)
    {
        printf("Infrared config file can't be read\n");
        return 0;
    }

    //Initiate I2C compass
    compass=wiringPiI2CSetup(compass_adr);
    if (compass==-1)
    {
        printf("Compass can't be initialised\n");
        return 0;
    }
    if (wiringPiI2CWriteReg8(compass,0x02,0x00)==-1) // continuous measurement mode
    {
        printf("Compass can't be initialised\n");
        return 0;
    }
    return 1;
}


/* Restore the keybord */

void bot_close(void)
{
    bot_motor_stop();
    if (bot_rest_terminal(&bot_keyboard)==-1)
        printf("Error during keyboard restoration\n");
    //Frees the data structures associated with config.
    lirc_freeconfig(config);
    //lirc_deinit() closes the connection to lircd and does some internal clean-up stuff.
    lirc_deinit();
}

void bot_wait(unsigned int how_long)
{
    delay(how_long);		// mS
}


/**********************************************************************************************************
 Sonar Management
***********************************************************************************************************/

/* Gives the distance  in cm from the front of the robot and an obstacle */

int bot_get_front_distance(void)
{
    long int start=0;
    digitalWrite(bot_front_sonar_trigger, HIGH);
    delayMicroseconds(20);
    digitalWrite(bot_front_sonar_trigger, LOW);
    while (digitalRead(bot_front_sonar_echo)==LOW);
    start = micros();
    while (digitalRead(bot_front_sonar_echo)==HIGH);
    return (micros()-start)/58;
}

int bot_get_left_distance(void)
{
    long int start=0;
    digitalWrite(bot_left_sonar_trigger, HIGH);
    delayMicroseconds(20);
    digitalWrite(bot_left_sonar_trigger, LOW);
    while (digitalRead(bot_left_sonar_echo)==LOW);
    start = micros();
    while (digitalRead(bot_left_sonar_echo)==HIGH);
    return (micros()-start)/58;
}

int bot_get_right_distance(void)
{
    long int start=0;
    digitalWrite(bot_right_sonar_trigger, HIGH);
    delayMicroseconds(20);
    digitalWrite(bot_right_sonar_trigger, LOW);
    while (digitalRead(bot_right_sonar_echo)==LOW);
    start = micros();
    while (digitalRead(bot_right_sonar_echo)==HIGH);
    return (micros()-start)/58;
}

/**********************************************************************************************************
 Power shield Management
***********************************************************************************************************/

/* Send a command to the power shield */

void bot_ps_send(const char *s, int nb)
{
    unsigned char data[2];
    unsigned char data2[1]= {'x'};
    int i;
    digitalWrite(10,LOW);
    for(i=0; i<strlen(s); i++)
    {
        data[0]=s[i];
        data[1]=0;
        //printf("%s\n",data);
        bot_wait(bot_spi_delay);
        wiringPiSPIDataRW(0,data,1);
    }
    if (strlen(s)==1)
        wiringPiSPIDataRW(0,data2,1);

    if (nb>0)
        wiringPiSPIDataRW(0,data2,1);
}

/* Read the answer from the power shield */

char * bot_ps_read(int nb)
{

    char  *s=NULL;
    s=malloc(sizeof(*s)*(nb+1));
    int i;
    for(i=0; i<nb; i++)
    {
        bot_wait(bot_spi_delay);
        unsigned char data[1]= {'x'};
        wiringPiSPIDataRW(0,data,1);
        s[i]=data[0];
        //printf("[%c]%d[%c]",data[0],i,s[i]);
    }
    digitalWrite(10,HIGH);
    s[nb]='\0';
    return s;
}

/* Read and write at the same time
s = string you wand to send length doesn't matter
nb number of char  you want to get */

char *bot_ps_RW(const char *s, int nb)
{
    bot_ps_send(s, 0);
    return bot_ps_read(nb);
}

/**********************************************************************************************************
 Sound and light Management
***********************************************************************************************************/

void bot_get_sensor(int *lft_snd, int *rgt_snd, int *lft_lgt, int *rgt_lgt)
{
    char *s;
    s=bot_ps_RW("A",8);
    /*int i;
    for (i=0; i<8; i++)
        printf("%d : %d\n",i,s[i]);*/
    *lft_snd=(s[1]<<8)|s[0]; // left sound sensor
    *rgt_snd=(s[3]<<8)|s[2]; // right sound sensor
    *lft_lgt=(s[5]<<8)|s[4]; // left light sensor
    *rgt_lgt=(s[7]<<8)|s[6]; // right light sensor
}

/**********************************************************************************************************
 keyboard Management
***********************************************************************************************************/

int bot_cnf_terminal (struct termios *prev)
{
    struct termios new;
    if (tcgetattr(fileno(stdin),prev)==-1)
    {
        perror("tcgetattr");
        return -1;
    }
    new.c_iflag=prev->c_iflag;
    new.c_oflag=prev->c_oflag;
    new.c_cflag=prev->c_cflag;
    new.c_lflag=0;
    new.c_cc[VMIN]=1;
    new.c_cc[VTIME]=0;
    if (tcsetattr(fileno(stdin),TCSANOW,&new)==-1)
    {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

int bot_rest_terminal (struct termios *prev)
{
    return tcsetattr(fileno(stdin),TCSANOW,prev);
}

/**********************************************************************************************************
 servo motor Management
***********************************************************************************************************/

void bot_sm_left(void)
{
    bot_ps_RW("H0",0);
}

void bot_sm_right(void)
{
    bot_ps_RW("H1",0);
}

void  bot_sm_up(void)
{
    bot_ps_RW("V1",0);
}

void  bot_sm_down(void)
{
    bot_ps_RW("V0",0);
}


/**********************************************************************************************************
 motor Management
***********************************************************************************************************/

void bot_motor_left(void)
{
    bot_ps_RW("3",0);
}

void bot_motor_right(void)
{
    bot_ps_RW("4",0);
}

void  bot_motor_advance(void)
{
    bot_ps_RW("1",0);
}

void  bot_motor_back(void)
{
    bot_ps_RW("2",0);
}

void  bot_motor_stop(void)
{
    bot_ps_RW("5",0);
}

/**********************************************************************************************************
 Encoded wheels Management
***********************************************************************************************************/

//* call back function to count the ticks
// left wheel
void interupt_LW (void)
{
    bot_encoder_l++;
    bot_wheel_l+=bot_wheel_circ;
}

// right wheel
void interupt_RW (void)
{
    bot_encoder_r++;
    bot_wheel_r+=bot_wheel_circ;
}

/**********************************************************************************************************
 IR reciever Management
***********************************************************************************************************/

int bot_get_ir_code(char *code)
{
    if(strstr (code,"KEY_0")) return KEY_0;
    if(strstr (code,"KEY_1")) return KEY_1;
    if(strstr (code,"KEY_2")) return KEY_2;
    if(strstr (code,"KEY_3")) return KEY_3;
    if(strstr (code,"KEY_4")) return KEY_4;
    if(strstr (code,"KEY_5")) return KEY_5;
    if(strstr (code,"KEY_6")) return KEY_6;
    if(strstr (code,"KEY_7")) return KEY_7;
    if(strstr (code,"KEY_8")) return KEY_8;
    if(strstr (code,"KEY_9")) return KEY_9;
    if(strstr (code,"KEY_POWER")) return KEY_POWER;
    if(strstr (code,"KEY_VOLUMEUP")) return KEY_VOLUMEUP;
    if(strstr (code,"KEY_VOLUMEDOWN")) return KEY_VOLUMEDOWN;
    if(strstr (code,"KEY_UP")) return KEY_UP;
    if(strstr (code,"KEY_DOWN")) return KEY_DOWN;
    if(strstr (code,"KEY_REWIND")) return KEY_REWIND;
    if(strstr (code,"BTN_FORWARD")) return BTN_FORWARD;
    if(strstr (code,"KEY_PLAYPAUSE")) return KEY_PLAYPAUSE;
    if(strstr (code,"KEY_F1")) return KEY_F1;
    if(strstr (code,"KEY_F2")) return KEY_F2;
    if(strstr (code,"KEY_F3")) return KEY_F3;
    return 99;
}

void bot_wait_IR_cmd(int key)
{
    char *code;
    int c;

    //Do stuff while LIRC socket is open 0=open -1=closed.
    while(lirc_nextcode(&code)==0)
    {
        //If code = NULL, meaning nothing was returned from LIRC socket,
        //then skip lines below and start while loop again.
        if(code==NULL) continue;
        {
            c=bot_get_ir_code(code);
            if (c==key) break;
        }
    }
}

int bot_wait_IR(void)
{
    char *code;

    //Do stuff while LIRC socket is open 0=open -1=closed.
    while(lirc_nextcode(&code)==0)
    {
        //If code = NULL, meaning nothing was returned from LIRC socket,
        //then skip lines below and start while loop again.
        if(code==NULL) continue;
        {
            return bot_get_ir_code(code);
        }
    }
    return 99;
}

void bot_get_IrKey (void)
{
    //Timer for our buttons
    int buttonTimer = millis();

    char *code;
    int c;

    //Do stuff while LIRC socket is open 0=open -1=closed.
    while(lirc_nextcode(&code)==0)
    {
        //If code = NULL, meaning nothing was returned from LIRC socket,
        //then skip lines below and start while loop again.
        if(code==NULL) continue;
        {
            //Make sure there is a 400ms gap before detecting button presses.
            if (millis() - buttonTimer > 400)
            {
                c=bot_get_ir_code(code);
                if (c!=99) buttonTimer = millis();
                switch (c)
                {
                case KEY_0 :
                    break;
                case KEY_1 :
                    break;
                case KEY_2 :
                    break;
                case KEY_3 :
                    break;
                case KEY_4 :
                    break;
                case KEY_5 :
                    break;
                case KEY_6 :
                    break;
                case KEY_7 :
                    break;
                case KEY_8 :
                    break;
                case KEY_9 :
                    break;
                case KEY_POWER  :
                    break;
                case KEY_VOLUMEUP  :
                    break;
                case KEY_VOLUMEDOWN :
                    break;
                case KEY_UP :
                    break;
                case KEY_DOWN :
                    break;
                case KEY_REWIND :
                    break;
                case BTN_FORWARD :
                    break;
                case KEY_PLAYPAUSE :
                    break;
                case KEY_F1 :
                    break;
                case KEY_F2 :
                    break;
                case KEY_F3  :
                    break;

                }
            }
            //Need to free up code before the next loop
            free(code);
        }
    }

}

/**********************************************************************************************************
 Compass Management
***********************************************************************************************************/

double bot_get_compass_xyz(short *x,short *y, short *z)
{

int xm,xl,ym,yl,zm,zl;
double angle;

xm=wiringPiI2CReadReg8(compass,0x03);
xl=wiringPiI2CReadReg8(compass,0x04);
zm=wiringPiI2CReadReg8(compass,0x05);
zl=wiringPiI2CReadReg8(compass,0x06);
ym=wiringPiI2CReadReg8(compass,0x07);
yl=wiringPiI2CReadReg8(compass,0x08);


*x=(xm<<8) | xl;
*y=(ym<<8) | yl;
*z=(zm<<8) | zl;

angle=atan2((double)*x,(double)*y)*180/M_PI+180;

/*
    *x=wiringPiI2CReadReg8(compass,0x03)<<8; // X msb
    *x|=wiringPiI2CReadReg8(compass,0x04); // X lsb
    *y=wiringPiI2CReadReg8(compass,0x05)<<8; // Y msb
    *y|=wiringPiI2CReadReg8(compass,0x06); // Z lsb
    *z=wiringPiI2CReadReg8(compass,0x07)<<8; // Z msb
    *z|=wiringPiI2CReadReg8(compass,0x08); // Z lsb



*x=wiringPiI2CReadReg16(compass,0x03);
*y=wiringPiI2CReadReg16(compass,0x05);
*z=wiringPiI2CReadReg16(compass,0x07); */

printf("A %d %d %d %f\n",*x,*y,*z,angle);
//The output range of the HMC5883L is -2048 to 2047 and the input range of the ATN function
//is -127 to 127.  I used the method to scale the output to the input discribed in Parallax's Smart
//SCALEFACTOR = 65536 * (255/4095) = 4081
//SCALEOFFSET = 2048
//ATNFACTOR = 127
//LOWRANGE = 0
//HIGRANGE = 4095 (2048 + 2047 = 4095)
/*
    *x = *x + SCALEOFFSET;
    *x = (*x<LOWRANGE?LOWRANGE:*x);
    *x = (*x>HIGHRANGE?HIGHRANGE:*x);
    *x = *x * SCALEFACTOR - ATNFACTOR;

    *y = *y + SCALEOFFSET;
    *y = (*y<LOWRANGE?LOWRANGE:*y);
    *y = (*y>HIGHRANGE?HIGHRANGE:*y);
    *y = *y * SCALEFACTOR - ATNFACTOR;*/
//printf("B %d %d %d %f\n",*x,*y,*z,atan2(*x,*y*-1)/361 );
    return angle;

}
