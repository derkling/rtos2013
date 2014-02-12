/**
 * File: statistics.cpp
 * \brief this class calculates the statistics of the training
 * 
 * \details  This class calculate the average speed, the consumed calories and 
 *                     the time in seconds for the training
 * \author Omar Scotti
 * \author Diego Rondelli
 * \date 21/01/2014
 * \version 1.0
 */

#include <cstdio>
#include "miosix.h"
#include "lis302dl.h"
#include "statistics.h"
#include "pedometer.h"
#include <math.h>
#include "utility.h"

using namespace std;
using namespace miosix;


float avg;
int oldSteps;
float cal;
float distance;
int seconds;
int stepsInInterval;
int height;
Pedometer* pedometer;
float multParam;
bool moving;
int walkTime;
int runTime;
int stopTime;
int newSteps;
float stride;

bool Statistics::instanceFlag = false;
Statistics* Statistics::statistics = NULL;
Utility* utility_st;

/**
 * \brief Private class constructor
 */
Statistics::Statistics(){
    utility_st=Utility::getInstance();
    pedometer = Pedometer::getInstance();
    avg=0;
    oldSteps = pedometer->getStep();
    cal=0;
    distance=0;
    seconds=0;
    stepsInInterval=0;
    height=170; //=getHeight(); from the interface module.
    multParam=1;
    moving=false;
    runTime=0;
    walkTime=0;
    stopTime=0;
    stride=0;
    newSteps=0;
}

/**
 * \brief Factory patten 
 * \details Factory pattern guarantees that in all the program only one object
 *          with type Utility exists. To make this the constructor is private
 *          and is substituted by this method
 * @param void
 * @returns the only existing instance of Statistics 
 */
Statistics* Statistics::getInstance()
{
    if(! instanceFlag)
    {
        statistics = new Statistics();
        instanceFlag = true;
        return statistics;
    }
    else
    {
        return statistics;
    }
}

/**
 * \brief starts the statistic calc
 * \details calculates the activity time in seconds, the average speed and the 
 *          distance.
 * @param void
 * @return void
 */
void Statistics::start(){
    while(true){
    
    usleep(1900000);
    newSteps=pedometer->getStep();
    stepsInInterval = (newSteps-oldSteps);

    switch(stepsInInterval){
            
        case (0): 
            moving=false;
            stopTime+=2;
            break;
            
        case (1): 
            moving=true;
            multParam=5;
            walkTime+=2;
            break;
        case (2): 
            moving=true;
            multParam=5;
            walkTime+=2;
            break;
        case (3):
            moving=true;
            walkTime+=2;
            multParam=5;
            break;
        case (4):
            moving=true;
            multParam=3;
            runTime+=2;
            break;
        case (5):
            moving=true;
            multParam=2;
            runTime+=2;
            break;
        case(6):
            moving=true;
            multParam=1.2;
            runTime+=2;
            break;
        case(7):
            moving=true;
            multParam=1;
            runTime+=2;
            break;
        default:
            moving=true;
            multParam=(1/1.2);
            runTime+=2;
    }
    
                       
    if(moving){
        stride = height/multParam;
        seconds+=2;
        distance+=(stride*stepsInInterval);
        avg=distance/seconds;
    } 
    oldSteps=newSteps;
    utility_st->test2(stepsInInterval,newSteps,distance,height,multParam,avg);
   }
}

/**
 *\brief  getter of average speed
 *@param void
 * @return the average speed 
 */
float Statistics::getAvg(){
    return avg;
}
/**
 * \brief getter of distance traveled
 *@param void
 * @return distance traveled
 */
float Statistics::getDistance(){
    return distance;
}
/**
 * \brief getter of total moving time
 * @return time of moving in seconds
 */
int Statistics::getTime(){
    return seconds;
}
/**
 * \brief getter of walking time
 * @return time of walking in seconds
 */
int Statistics::getWalkTime(){
    return walkTime;
}
/**
 * \brief getter of run time
 * @return time of run in seconds
 */
int Statistics::getRunTime(){
    return runTime;
}
/**
 * \brief getter of pause time
 * @return time of pause in seconds
 */
int Statistics::getStopTime(){
    return stopTime;
}