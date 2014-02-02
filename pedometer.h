/**
 * File:   pedometer.h
 *  \brief     This file is the header file for the class pedometer
 * 
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      10/01/2014
 */

#ifndef PEDOMETER_H
#define	PEDOMETER_H
#include "types.h"
#include <vector>
class Pedometer
{
public:
    
   static Pedometer* getInstance();
    
   void start();
   
   int getStep();
   
   ~Pedometer()
    {
        instanceFlag = false;
    }
   
private:
    
   Pedometer();
    
   static Pedometer *pedometer;
    
   static bool instanceFlag;
    
   struct Average
        {
        int16_t x;
        int16_t y;
        int16_t z;
        }average4,average16;
   
   struct Data
        {
        int16_t lastDataX[16];
        int16_t lastDataY[16];
        int16_t lastDataZ[16];
        }data;
   
   void stepCounter();
   
   void stepCounter2();
   
   void takeAverage();
   
   void average(Average* average,int num);
   
   void accMax();
   
   void incrementStep();
   
   void updateData();
   
   void mediaPrint();
   
   void dataXPrint();
   
   void dataYPrint();
   
   void dataZPrint();
};

#endif	/* PEDOMETER_H */

