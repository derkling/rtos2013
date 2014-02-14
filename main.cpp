
/**
 * File:   lis302dl.cpp
 *  \brief     main class. It only starts the program
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      14/01/2014
 */

#include <cstdlib>
#include "pedometer/pedometer.h"
#include "pedometer/statistics.h"
#include <pthread.h>
#include "miosix.h"
#include "social/spi.h"
#include "social/social_wireless.h"
#include <cstdio>
#include <unistd.h>
#include "button.h"

using namespace std;
using namespace miosix;

typedef Gpio<GPIOD_BASE,15> blueLed;

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
 * \brief Main method, starts the pedometer
 */
int main(int argc, char** argv) {
//    char payload[33];
//    blueLed::mode(Mode::OUTPUT);
//
//
//    char orangeon[]="orangeon";
//
//    char orangeoff[]="orangeoff";
//
//    blueLed::high();
//
//    while(1){
//        scanf("%s",payload);
//        sendData(payload);		
////		sendData(orangeon);
////		sendData(orangeoff);
////		usleep(50000);
//    }
  
    init();
    pedometerApp = Pedometer::getInstance();
    statistics = Statistics::getInstance();
    
    pthread_t statisticsThread;
    pthread_create(&statisticsThread,NULL,&startStatistics,NULL);
    
    pthread_t pedometerThread;
    pthread_create(&pedometerThread,NULL,&startPedometer,NULL);
    
    pthread_join(pedometerThread,NULL);
    pthread_join(statisticsThread,NULL);
    
    configureButtonInterrupt();
	for(;;)
	{
		waitForButton();
		printf("Pulsante premuto\n");

	   
		int dati_pedometro_int=pedometerApp->getStep();
		char dati_pedometro_string[3];
		sprintf(dati_pedometro_string, "%d", dati_pedometro_int);
		printf("%s",dati_pedometro_string);
		sendData(dati_pedometro_string);
		
	}
    
    
    return 0;
}


