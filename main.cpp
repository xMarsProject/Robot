#include <iostream>
#include "driver.h"
#include "test.hpp"

using namespace std;

int main()
{
    cout<<"Bot V1"<<endl;
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

