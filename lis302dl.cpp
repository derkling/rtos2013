/**
 * File:   lis302dl.cpp
 *  \brief     This class contains the methods to sets up the accelerometer and
 *             read his data
 *  \details   This class contains 3 public methods:
 *             -lis302dl() that is the class constructor
 *             -memsConfig() that write on the accelerometer registers the 
 *             configuration values
 *             -getMemsData() that puts in three variables the values of 
 *             accelerations for the x,y and z axes
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      03/01/2014
 */
#include <cstdio>
#include "miosix.h"
#include "lis302dl.h"
#include "spi1.h"
#include "lis302dl_reg.h"
#include "utility.h"

using namespace std;
using namespace miosix;
Spi spi;
Utility* utility_l;

/**
 * \brief Class constructor
 */
Lis302dl::Lis302dl(){
    utility_l=Utility::getInstance();
}
/**
 * \brief this function configure the SPI and write on it the configuration parameters
 * for the accelerometer
 * @param void
 * @returns void
 */
void Lis302dl::memsConfig(){
    uint8_t val=0x00;
    spi.config();
    val = CTRL_REG1_XEN | CTRL_REG1_YEN  | CTRL_REG1_ZEN | CTRL_REG1_FSEN
            | CTRL_REG1_PDEN;
    spi.write(CTRL_REG1, &val, 1);
}
/**
 * \brief This function perform a read for the accelerations for x,y,z axes
 * @param x pointer to x variable
 * @param y pointer to y variable
 * @param z pointer to z variable
 * @return void
 */
void  Lis302dl::getMemsData(int16_t* x, int16_t* y, int16_t* z)
{	
	/* Read out all 6 bytes in one shot */
	*x = spi.singleRead(OUT_X);
        *y = spi.singleRead(OUT_Y);
        *z = spi.singleRead(OUT_Z);
}
