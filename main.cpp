#include <iostream>
#include "driver.h"
#include "test.hpp"

using namespace std;

int main()
{
    cout<<"Bot V2"<<endl;
    if (!bot_init())
    {
        cout<<"Error ... End of program"<<endl;
        return 0;
    }
    test();
    bot_close();
    cout<<"End of program"<<endl;
    return 0;
}

void drive_and_stop(void)
{
    bot_motor_advance();
    while(1)
    {
        if (bot_get_front_distance()<10)
        bot_motor_stop();
    }
}
