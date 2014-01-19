/**
 * File:   utility.cpp
 *  \brief     This class contains the methods to initialize and to make the
 *             blue, red and green led blink
 * 
 *             This class contains a method to initialize red, blue and green led,
 *             a method for each led to make it blink one time and a method to 
 *             print the variables of the program on serial peripheral.
 *             This class is only for debug, it will not included on the final 
 *             version of the project
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      16/01/2014
 */

#include "utility.h"
#include <iostream>
#include <cstdio>
#include "miosix.h"
#include <string.h>

using namespace std;
using namespace miosix;

bool Utility::instanceFlag = false;
Utility* Utility::utility = NULL;
typedef Gpio<GPIOD_BASE,15> blueLed;
typedef Gpio<GPIOD_BASE,14> redLed;
typedef Gpio<GPIOD_BASE,12> greenLed;

/*private constructor for the class*/
Utility::Utility(){
    blueLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    greenLed::mode(Mode::OUTPUT);
    
}

/**
 * \brief Factory patten 
 * \details Factory pattern guarantees that in all the program only one object
 *          with type Utility exists. To make this the constructor is private
 *          and is substituted by this method
 * @param void
 * @returns the only existing instance of Utility 
 */
Utility* Utility::getInstance()
{
    if(! instanceFlag)
    {
        utility = new Utility();
        instanceFlag = true;
        return utility;
    }
    else
    {
        return utility;
    }
}

/**
 * make the blue led blink one time
 * @param void
 * @return void
 */
void Utility::ledBlue(){
    blueLed::high();
    usleep(100000);
    blueLed::low();
    usleep(100000);
}

/**
 * make the red led blink one time
 * @param void
 * @return void
 */
void Utility::ledRed(){
    redLed::high();
    usleep(100000);
    redLed::low();
    usleep(100000);
}

/**
 * make the green led blink one time
 * @param void
 * @return void
 */
void Utility::ledGreen(){
    greenLed::high();
    usleep(100000);
    greenLed::low();
    usleep(100000);
}

/**
 * print the program variables on serial interface
 * @param x the x-axe acceleration
 * @param y the y-axe acceleration
 * @param z the z-axe acceleration
 * @param step actual step number
 * @param test
 * @return void
 */
void Utility::test(int16_t x,int16_t y,int16_t z,int step,int accelleration,int aMax, int limit){
    
    printf("\ndx: %d\t",x);
    printf("dy: %d\t",y);
    printf("dz: %d\t",z);
    printf("aMax: %i\t",aMax);
    printf("limit: %i\t",limit);
    printf("accelleration: %i\t",accelleration);
    printf("step: %i",step);
    
}