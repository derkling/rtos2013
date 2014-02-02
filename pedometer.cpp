/**
 * File:   pedometer.cpp
 *  \brief     This class contains the logic to understand if a movement is a step
 *             or not

 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      10/01/2014
 */

#include <cstdio>
#include "miosix.h"
#include "lis302dl.h"
#include "pedometer.h"
#include "utility.h"
#include <math.h>

#define LIMIT   155
#define R       3
#define RANGE   256

using namespace std;
using namespace miosix;

/**
 * Defines the state of the counter
 */
typedef enum
   {
   onPause = 0,
   onMove  = 1
    } tState ;

int step,xt,yt,zt,limit,accelleration,aMax,dataLimit;
int16_t x, y, z;
Lis302dl lis302dl;
tState currentState= onPause;
Utility* utility_p;

bool Pedometer::instanceFlag = false;
Pedometer* Pedometer::pedometer = NULL;

/**
 * Private class constructor
 */
Pedometer::Pedometer(){
    utility_p=Utility::getInstance();
    
    average4.x=0;
    average4.y=0;
    average4.z=0;
    average16.x=0;
    average16.y=0;
    average16.z=0;
    
    for(int i=0;i<16;i++){
        data.lastDataX[i]=0;
        data.lastDataY[i]=0;
        data.lastDataZ[i]=0;
    }
    xt=0;
    yt=0;
    zt=0;
    step=0;
    aMax=LIMIT+15;
    accelleration=0;
    limit=LIMIT;
}

/**
 * \brief Factory patten 
 * \details Factory pattern guarantees that in all the program only one object
 *          with type Utility exists. To make this the constructor is private
 *          and is substituted by this method
 * @param void
 * @returns the only existing instance of Pedometer 
 */
Pedometer* Pedometer::getInstance(){
        if(! instanceFlag)
    {
        pedometer = new Pedometer();
        instanceFlag = true;
        return pedometer;
    }
    else
    {
        return pedometer;
    }
    
}

/**
 * Starts the step counting
 * @param void
 * @returns void
 */
void Pedometer::start(){
    
    utility_p->ledGreen();
    
    lis302dl.memsConfig();
    
    utility_p->ledRed();
            
    while(true){
        
        takeAverage();
        
        stepCounter();
        }
}


void Pedometer::stepCounter(){
        
        //Step are considered as linked to fast variation of the acceleration.
        //We first evaluate the variation (dx/dy/dz) by substracting the current
        //values given by the MEMS to an 'older' value, in fact another average value 
        //done with more value (longer time constant).
        //delta_x = average on last 4 value, minus average done on last 16 value
        unsigned dx =  average4.x - average16.x ; 
        unsigned dy =  average4.y - average16.y ; 
        unsigned dz =  average4.z - average16.z ; 
        
        //Calcolo il modulo dell'accellerazione
        float accellerationF = sqrt( (float)(dx*dx + dy*dy + dz*dz )) ;  
        accelleration=(int)(accellerationF+0.5);
        
        //Setto il valore massimo ed il minimo dell'accellerazione
        accMax();
        
        
        dataLimit++;
                    
        //Ogni 100 valori ricalcolo il limite. Limite dinamico
        if(dataLimit==100){
                        limit=aMax-15;
                        aMax=LIMIT+15;
                        dataLimit=0;
                    }
        

        //Now we switch from the 'OnMove' state to 'On Pause' (and vice versa) with a small hysteresis
        if ( (accelleration< limit ) && (currentState == onMove) )
                {
                currentState = onPause;
                
                if((fabs(x-xt)<=R || fabs(x-xt)>=RANGE-R) && (fabs(y-yt)<=R || fabs(y-yt)>=RANGE-R) && (fabs(z-zt)<=R || fabs(z-zt)>=RANGE-R)){
                    incrementStep();
                 }
         }
        else if ( (accelleration>=limit ) && (currentState == onPause))
                {
                currentState = onMove;
                xt=x;
                yt=y;
                zt=z;
                }
        
        utility_p->test(x,y,z,step,accelleration,aMax,limit);
        
}

void Pedometer::takeAverage(){
    lis302dl.getMemsData(&x,&y,&z);
    updateData();
    average(&average4,4);
    average(&average16,16);
}

void Pedometer::updateData(){
    
    /*Shift a destra*/
    for(int i=15;i>0;i--){
        data.lastDataX[i]=data.lastDataX[i-1];
        data.lastDataY[i]=data.lastDataY[i-1];
        data.lastDataZ[i]=data.lastDataZ[i-1];
    }
    
    /*Aggiunta nuovo dato*/
    data.lastDataX[0]=x;
    data.lastDataY[0]=y;
    data.lastDataZ[0]=z;
}

void Pedometer::average(Average* average,int num){
    int sumX=0,sumY=0,sumZ=0;
    
    /*Somma*/
    for(int i=0;i<num;i++){
        sumX +=data.lastDataX[i];
        sumY +=data.lastDataY[i];
        sumZ +=data.lastDataZ[i];
    }
    
    /*Media*/
    float averageXf=(float)sumX/(float)num;
    float averageYf=(float)sumY/(float)num;
    float averageZf=(float)sumZ/(float)num;
   
    average->x=(int16_t)(averageXf+0.5);
    average->y=(int16_t)(averageYf+0.5);
    average->z=(int16_t)(averageZf+0.5);
}

void Pedometer::accMax(){
    if(accelleration>aMax) aMax=accelleration;
}

void Pedometer::incrementStep(){
    step++;
    //if(!(step%50)) evviva(step);
}

/**
 * getter of actual steps number
 * @param void
 * @returns int number of steps
 */
int Pedometer::getStep(){
    return step;
}

/**
 * print on serial peripheral the values of the average acceleration on the 3 axes
 * debug only
 */
void Pedometer::mediaPrint(){
        printf("\nmedia4x: %i\t",average4.x);
        printf("media16x: %i\t",average16.x);
        printf("media4y: %i\t",average4.y);
        printf("media16y: %i\t",average16.y);
        printf("media4z: %i\t",average4.z);
        printf("media16z: %i\t",average16.z);
}

/**
 * print on serial peripheral the acceleration for the x axe
 * debug only
 */
void Pedometer::dataXPrint(){
    printf("x: %i dataX: ",x);
    for(int i=0;i<15;i++){
        printf("%i",data.lastDataX[i]);
    }
    printf("\n");
}

/**
 * print on serial peripheral the acceleration for the y axe
 * debug only
 */
void Pedometer::dataYPrint(){
    printf("y: %i dataY: ",y);
    for(int i=0;i<15;i++){
        printf("%i",data.lastDataY[i]);
    }
    printf("\n");
}

/**
 * print on serial peripheral the acceleration for the z axe
 * debug only
 */
void Pedometer::dataZPrint(){
    printf("z: %i dataZ: ",z);
    for(int i=0;i<15;i++){
        printf("%i",data.lastDataZ[i]);
    }
    printf("\n");
}
