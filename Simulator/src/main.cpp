#include <pthread.h>
#include "Robot.h"
#include "Simulator.h"

void *RunBot(void *arg){
      Robot *Bot=(Robot*)arg;
      Bot->Run();
      return NULL;
}

void *RunSimulator(void *arg){
      Simulator *Simu = (Simulator*)arg;
      Simu->Run();
      return NULL;
}

int main(int argc, char* args[] ){
    pthread_t robot, simulator;

    Robot *Bot=new Robot();
    Simulator *Simu=new Simulator(640,480,1000,1);
    Simu->Bot=Bot;
    pthread_create( &robot, NULL, RunBot, (void*) Bot);
    pthread_create( &simulator, NULL, RunSimulator, (void*) Simu);
    pthread_join(robot,NULL);
    pthread_join(simulator,NULL);

    return 0;
}
