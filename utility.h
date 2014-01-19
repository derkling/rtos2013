/**
 * File:   Utility.h
 *  \author     Omar Scotti
 *  \author     Diego Rondelli
 *  \brief      This file is the header file for the Utility class 
 *  \version    1.0
 *  \date       16/01/2014 
 */

#ifndef UTILITY_H
#define	UTILITY_H
#include "types.h"

class Utility
{
private:
    static bool instanceFlag;
    static Utility *utility;
    Utility();
    
public:
    static Utility* getInstance();
    void ledBlue();
    void ledRed();
    void ledGreen();
    void test(int16_t x,int16_t y,int16_t z,int step,int accelleration,int aMax, int limit);
    ~Utility()
    {
        instanceFlag = false;
    }
};

#endif	/* UTILITY_H */

