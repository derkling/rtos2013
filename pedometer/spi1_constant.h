/**
 *  File:      spi1_constant.h
 *  \brief     This file contains the definition of all the constants used in
 *             the spi1 class
 *  \details   
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      10/01/2014
 */

#ifndef ADDRESS_B_H
#define	ADDRESS_B_H

#define SPI_MULTI_OP            1<<6
#define SPI_READ                1<<7
#define SPI_Direction           (uint16_t)0x0000
#define SPI_Mode                (uint16_t)0x0104
#define SPI_DataSize            (uint16_t)0x0000
#define SPI_CPOL                (uint16_t)0x0000
#define SPI_CPHA                (uint16_t)0x0000
#define SPI_NSS                 (uint16_t)0x0200
#define SPI_BaudRatePrescaler   (uint16_t)0x0008
#define SPI_FirstBit            (uint16_t)0x0000
#define SPI_CRCPolynomial       7

#endif	/* ADDRESS_B_H */

