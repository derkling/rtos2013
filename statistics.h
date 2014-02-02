/**
 * File:   pedometer.h
 *  \brief     This file is the header file for the class statistics
 * 
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      21/01/2014
 */

#ifndef STATISTICS_H
#define	STATISTICS_H
class Statistics{
public:
    
    static Statistics* getInstance();
    
    void start();
    
    float getAvg();
    
    float getCal();
    
    int getTime();
    
    float getDistance();
    
    int getRunTime();
    
    int getWalkTime();
    
    int getStopTime();
    
        ~Statistics()
    {
        instanceFlag = false;
    }

private:
    
    Statistics();
    
    static Statistics* statistics;
    static bool instanceFlag;
    float avg;
    int oldSteps;
    float distance;
    int seconds;
    int stepsInInterval;
    int height;
    float multParam;
    bool moving;
    int runTime;
    int walkTime;
    int stopTime;
    float stride;
           
};



#endif	/* STATISTICS_H */

