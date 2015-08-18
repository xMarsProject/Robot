#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>
#include <math.h>

#include "driver.h"

/* initialize the robot, all the components are  initialised even if you won't use them */
unsigned long int bot_encoder_l; // tick counter for left wheel
unsigned long int bot_encoder_r; // tock counter for right wheel
float bot_wheel_l; // number of cm for the lef wheel
float bot_wheel_r;// number of cm for the right wheel


int bot_ps; // SPI channel to communicate with the power shield
double xg, yg, zg; // accelerometer

struct termios bot_keyboard;

int bot_init()
{
    if (bot_cnf_terminal(&bot_keyboard)==-1) // configure the terminal to use the keybord key by key
    {
        printf("Keyboard can't be initialized ...\n");
        return 0;
    }

    wiringPiSetup();
    // LEDs
    pinMode(bot_RLED, OUTPUT);
    pinMode(bot_GLED, OUTPUT);
    // button
    pinMode(bot_button, INPUT);
    // sonar
    pinMode(bot_front_sonar_trigger,OUTPUT);
    pinMode(bot_front_sonar_echo,INPUT);
    digitalWrite(bot_front_sonar_trigger,HIGH);
    // Power shield
    pinMode(10, OUTPUT);

    bot_ps = wiringPiSPISetup (0,1000000);
    if (bot_ps==-1)
    {
        printf("Power shield can't be initialised\n");
        return 0;
    }
    xg=yg=zg=0;
    bot_motor_stop();
    // encoded wheels
    pinMode(21, INPUT);
    pinMode(0, INPUT);
    bot_encoder_l=0;
    bot_encoder_r=0;
    bot_wheel_l=0;
    bot_wheel_r=0;
    wiringPiISR(0,INT_EDGE_RISING,&interupt_p0); // set interupt to count  the ticks of the left wheel
    wiringPiISR(21,INT_EDGE_RISING,&interupt_p21); // set interupt to count the ticks of the right wheel
    return 1;
}


/* Restore the keybord */

void bot_close(void)
{
    bot_motor_stop();
    if (bot_rest_terminal(&bot_keyboard)==-1)
        printf("Error during keyboard restoration\n");
}

/**********************************************************************************************************
 LED Management
***********************************************************************************************************/

void bot_red_led_on(void)
{
    digitalWrite (bot_RLED, LOW);
}

void bot_red_led_off(void)
{
    digitalWrite (bot_RLED,HIGH);
}

void bot_green_led_on(void)
{
    digitalWrite (bot_GLED, LOW);
}

void bot_green_led_off(void)
{
    digitalWrite(bot_GLED, HIGH);
}

void bot_wait(unsigned int how_long)
{
    delay(how_long);		// mS
}

/* makes the both leds blinking nb times,used to check if the wiring is ok */

void bot_blink(int nb)
{
    int i=0;
    for (i=0; i<nb; i++)
    {
        bot_red_led_on();
        bot_green_led_on();
        bot_wait(500);
        bot_red_led_off();
        bot_green_led_off();
        bot_wait(500);
    }
}

/**********************************************************************************************************
 Button Management
***********************************************************************************************************/

/* tells if the button on the board is pushed */

int bot_button_is_pushed(void)
{
    return  !digitalRead(bot_button);
}

/* waits until the button on the board is pushed */

void bot_wait_button(void)
{
    while (digitalRead(bot_button)==HIGH)
        bot_wait(100);
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
    bot_ps_send(s, nb);
    return bot_ps_read(nb);
}


/* Make the led blink on the power shield, used to check if the communication is ok */

void bot_ps_blink(int nb)
{
    int i=0;
    for (i=0; i<nb; i++)
    {
        bot_ps_RW("L1",0);
        bot_wait(500);
        bot_ps_RW("L0",0);
        bot_wait(500);
    }
}

/**********************************************************************************************************
 keyboard Management
***********************************************************************************************************/

/* cette fonction reconfigure le terminal, et stocke */
/* la configuration initiale a l'adresse prev */

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

/* cette fonction restaure le terminal avec la */
/* configuration stockee a l'adresse prev */
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
 accelerometer Management
***********************************************************************************************************/

void bot_read_accel(double *pitch, double *roll)
{
    char *s;
    int x,y,z;
    double fXg, fYg, fZg;
    double xoffset=0;
    double yoffset=0;
    double zoffset=0;
    double rx, ry, rz;

    s=bot_ps_RW("A",3);
    //printf("x:%d° y=%d z=%d",s[0],s[1],s[2]);
    x=s[0];
    y=s[1];
    z=s[2];
    fXg =x * 0.00390625 + xoffset;
    fYg =y * 0.00390625 + yoffset;
    fZg =z * 0.00390625 + zoffset;
    rx = fXg * ALPHA + (xg * (1.0-ALPHA));
    xg = rx;
    ry = fYg * ALPHA + (yg * (1.0-ALPHA));
    yg = ry;
    rz = fZg * ALPHA + (zg * (1.0-ALPHA));
    zg = rz;
    *pitch = (atan2(xg,sqrt(yg*yg+zg*zg)) * 180.0) / M_PI;
    *roll = (atan2(yg,(sqrt(xg*xg+zg*zg))) * 180.0) / M_PI;
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
void interupt_p0 (void)
{
    bot_encoder_l++;
    bot_wheel_l+=bot_wheel_circ;
}


// right wheel
void interupt_p21 (void)
{
    bot_encoder_r++;
    bot_wheel_r+=bot_wheel_circ;
}
