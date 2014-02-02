/**
 * File:   lis302dl.cpp
 *  \brief     main class. It only starts the program
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      14/01/2014
 */

#include <cstdlib>
#include "pedometer.h"
#include "statistics.h"
#include <pthread.h>
#include "miosix.h"

Pedometer* pedometerApp;
Statistics* statistics;
#define PRIORITY_MAX 2

void *startPedometer(void *arg){
    miosix::Thread::getCurrentThread()->setPriority(PRIORITY_MAX-1);
    pedometerApp->start();
}

void *startStatistics(void *arg){
    miosix::Thread::getCurrentThread()->setPriority(PRIORITY_MAX-2);
    statistics->start();
}

/*
 * Main method, starts the pedometer
 */
int main(int argc, char** argv) {
    
    pedometerApp = Pedometer::getInstance();
    statistics = Statistics::getInstance();
    
    pthread_t statisticsThread;
    pthread_create(&statisticsThread,NULL,&startStatistics,NULL);
    
    pthread_t pedometerThread;
    pthread_create(&pedometerThread,NULL,&startPedometer,NULL);
    
    pthread_join(pedometerThread,NULL);
    pthread_join(statisticsThread,NULL);
    
    return 0;
}

