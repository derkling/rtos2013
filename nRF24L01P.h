/* 
 * File:   nRF24L01P.h
 * Author: r0rshark
 *
 * Created on 3 gennaio 2014, 17.22
 */
#include "spi_driver.h"

#ifndef NRF24L01P_H
#define	NRF24L01P_H


class nRF24L01P {
public:
    nRF24L01P();
    nRF24L01P(const nRF24L01P& orig);
    virtual ~nRF24L01P();    
    void init();
    void transmit(int num_passi);
    int receive();
    void waitForModule();
private:
    int get_register(int registro);
    spi_driver *spi;

};

#endif	/* NRF24L01P_H */

