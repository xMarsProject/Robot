#ifndef DRIVER_H_INCLUDED
#include <termio.h>
// pin wiring on the raspberry
// leds
#define bot_GLED 1
#define bot_RLED 29
//buttons
#define bot_button 28
//Sonar
#define bot_front_sonar_trigger 25
#define bot_front_sonar_echo    24
// power shield
#define bot_spi_delay 20 // delay for the communication on Raspberry/SPI with the power shield
// accelerometer
#define ALPHA 0.5
// encoded wheel
#define tick_axes 10 // number of ticks to complete a turn
#define bot_wheel_circ (6.5*M_PI/10) // number of cm for a tick (6.5 is the wheel diameter)


typedef struct { float x; float y; float z;} axes_t;
#ifdef __cplusplus
extern unsigned long int bot_encoder_l; // tick counter for left wheel
extern unsigned long int bot_encoder_r; // tock counter for right wheel
extern float bot_wheel_l; // number of cm for the lef wheel
extern float bot_wheel_r;// number of cm for the right wheel


extern int bot_ps; // SPI channel to communicate with the power shield
extern double xg, yg, zg; // accelerometer

extern struct termios bot_keyboard;
extern "C" {
#endif
int bot_init(void);
void bot_close(void);
void bot_red_led_on(void);
void bot_red_led_off(void);
void bot_green_led_on(void);
void bot_green_led_off(void);
void bot_blink(int);
void bot_wait_button(void);
int bot_button_is_pushed(void);
void bot_wait(unsigned int);
int bot_get_front_distance(void);
int bot_ps_echo(int);
void bot_ps_blink(int);
void bot_ps_send(const char *,int);
char * bot_ps_read(int);
char *bot_ps_RW(const char *s, int nb);
int bot_cnf_terminal (struct termios *);
int bot_rest_terminal (struct termios *);
void bot_sm_left(void);
void bot_sm_right(void);
void bot_sm_up(void);
void bot_sm_down(void);
void bot_read_accel(double *, double *);
void bot_motor_left(void);
void bot_motor_right(void);
void  bot_motor_advance(void);
void  bot_motor_back(void);
void  bot_motor_stop(void);
void interupt_p0 (void);
void interupt_p21 (void);
#ifdef __cplusplus
}
#endif

#define DRIVER_H_INCLUDED
#endif // DRIVER_H_INCLUDED
