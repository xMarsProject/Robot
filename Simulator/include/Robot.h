#ifndef ROBOT_H
#define ROBOT_H

class Robot
{
public:
    Robot();
    void Run();
    virtual ~Robot();
    bool Stop=false;
    bool stop=false;
    bool advance=false;
    bool left=false;
    bool right=false;
    bool bback=false;
    int ScanFront;
    int ScanRight;
    int ScanLeft;
    float bot_wheel_l=0;
    float bot_wheel_r=0;
    int azimuth=0;

protected:
private:
    void do_bot();
    void Bot_Stop();
    void bot_reach_angle(int angle, int round);
    void Bot_TrunRight(int angle, int round);
    void Bot_TrunLeft(int angle, int round);
    void bot_motor_advance();
    void bot_motor_back();
    void bot_motor_stop();
    void bot_motor_left();
    void bot_motor_right();
    void bot_close();
    void bot_wait(int t);
    int bot_get_front_distance();
    int bot_get_right_distance();
    int bot_get_left_distance();
    bool bot_init();
    void FollowWall(int dir);
    bool in(int a,  int b, int c);
};

#endif // ROBOT_H
