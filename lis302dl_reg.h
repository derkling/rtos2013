/**
 * File:   lis302dl_reg.h
 *  \brief     This file contains all the constants used in lis302dl class, 
 *             included the value used to configure the accelerometer register
 *  \details   
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      10/01/2014
 */

#ifndef LIS302DL_REG_H
#define	LIS302DL_REG_H

#define STATUS_REG      0x27
#define OUT_X          0x29
#define OUT_Y          0x2B
#define OUT_Z          0x2D

#define CTRL_REG1        0x20
#define CTRL_REG1_XEN    1<<0
#define CTRL_REG1_YEN    1<<1
#define CTRL_REG1_ZEN    1<<2
/*Scale selection*/
#define CTRL_REG1_FSEN   1<<5
/*Power down*/
#define CTRL_REG1_PDEN   1<<6

#endif	/* LIS302DL_REG_H */

