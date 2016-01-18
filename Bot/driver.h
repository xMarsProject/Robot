#ifndef DRIVER_H_INCLUDED
#include <termio.h>
// pin wiring on the raspberry
//Sonar
#define bot_front_sonar_trigger 25
#define bot_front_sonar_echo    24
#define bot_left_sonar_trigger  22
#define bot_left_sonar_echo     23
#define bot_right_sonar_trigger 04
#define bot_right_sonar_echo    05
//encoded wheel
#define bot_left_encoded_wheel  00
#define bot_right_encoded_wheel 21
// power shield
#define bot_spi_cmd             10
#define bot_MOSI                12
#define bot_MISO                13
#define bot_SCLK                14
//Infrared Reciever
#define bot_IR_R                01
//Infrared Sender
#define bot_IR_S                26
//compass
#define bot_compass_I2C_SCL     09
#define bot_compass_I2C_SDA     08

//keys for IR remote
#define KEY_0               1
#define KEY_1               2
#define KEY_2               3
#define KEY_3               4
#define KEY_4               5
#define KEY_5               6
#define KEY_6               7
#define KEY_7               8
#define KEY_8               9
#define KEY_9               10
#define KEY_POWER           11
#define KEY_VOLUMEUP        12
#define KEY_VOLUMEDOWN      13
#define KEY_UP              14
#define KEY_DOWN            15
#define KEY_REWIND          16
#define BTN_FORWARD         17
#define KEY_PLAYPAUSE       18
#define KEY_F1              19
#define KEY_F2              20
#define KEY_F3              21

#define bot_spi_delay 20 // delay for the communication on Raspberry/SPI with the power shield
// encoded wheel
#define tick_axes 10 // number of ticks to complete a turn
#define wheel_diameter 6.5
#define bot_wheel_circ (wheel_diameter*3.14/tick_axes) // number of cm for a tick

// compass parameter
#define compass_adr 0x1E
#define SCALEFACTOR   4081 //SCALEFACTOR = 65536 * (255/4095) = 4081
#define SCALEOFFSET   2048 //The output range of the HMC5883L is -2048 to 2047; therefore to get a 0 (LOWRANGE)to 4095 range
                           //to 4095 (HIGHRANGE) range 2048 must be added to the sensor value.
#define ATNFACTOR     127  //the ATN function input range is -127 TO 127. In order to get the positive scaled range of
                           //0 to 255 into the range of -127 to 127, 127 must be subtracted.
#define LOWRANGE      0    //see SCALEOFFSET
#define HIGHRANGE     4095 //see SCALEOFFSET

#ifdef __cplusplus
extern unsigned long int bot_encoder_l; // tick counter for left wheel
extern unsigned long int bot_encoder_r; // tock counter for right wheel
extern float bot_wheel_l; // number of cm for the lef wheel
extern float bot_wheel_r;// number of cm for the right wheel


extern struct termios bot_keyboard;
extern "C" {
#endif
int bot_init(void);
void bot_close(void);
void bot_wait(unsigned int);
int bot_get_front_distance(void);
int bot_get_left_distance(void);
int bot_get_right_distance(void);
void bot_ps_send(const char *,int);
char * bot_ps_read(int);
char *bot_ps_RW(const char *s, int nb);
int bot_cnf_terminal (struct termios *);
int bot_rest_terminal (struct termios *);
void bot_sm_left(void);
void bot_sm_right(void);
void bot_sm_up(void);
void bot_sm_down(void);
void bot_motor_left(void);
void bot_motor_right(void);
void bot_motor_advance(void);
void bot_motor_back(void);
void bot_motor_stop(void);
void interupt_LW (void);
void interupt_RW (void);
void bot_get_sensor(int *lft_snd, int *rgt_snd, int *lft_lgt, int *rgt_lgt);
void bot_get_IrKey(void);
int bot_get_ir_code(char *code);
void bot_wait_IR_cmd(int key);
int bot_wait_IR(void);
char *bot_wait_IR_c(void);
char *bot_get_ir_ccode(char *code);
double bot_get_compass_xyz(short *x,short *y, short *z);
#ifdef __cplusplus
}
#endif

#define DRIVER_H_INCLUDED
#endif // DRIVER_H_INCLUDED
