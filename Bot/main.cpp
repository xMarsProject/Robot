#include <iostream>
#include "driver.h"
#include "test.hpp"

using namespace std;

void do_bot(void)
{
        int obstacle=9999;
    bot_motor_advance();
    while ( obstacle>100 )
    {
        obstacle=bot_get_front_distance();
        bot_wait(250);
    }
    std::cout << "Obstacle :" << obstacle << std :: endl;
    bot_motor_stop();
}

int main()
{
    cout<<"Bot V2"<<endl;
    if (!bot_init())
    {
        cout<<"Error ... End of program"<<endl;
        return 0;
    }
    test();
    //do_bot();
    bot_close();
    cout<<"End of program"<<endl;
    return 0;
}



