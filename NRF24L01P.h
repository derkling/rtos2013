/* 
 * File:   NRF24L01P.h
 * Author: degrigis
 *
 * Created on January 9, 2014, 7:38 PM
 */

#ifndef NRF24L01P_H
#define	NRF24L01P_H
#include "spi.h"

class NRF24L01P {
public:
    NRF24L01P();
    NRF24L01P(const NRF24L01P& orig);
    virtual ~NRF24L01P();
    void writeRegister(unsigned char regAddress, unsigned char regData);
    unsigned char readRegister(unsigned char regAddress);
    unsigned char readStatusRegister();
    void powerUp();
    void powerDown();
    void setReceiveMode();
    void setTransmitMode();
    void returnStandByI();
    void sendDataToTx(int data);
    int receiveDataFromRx();
    unsigned char readRPD();
    void configureInterrupt();
private:
    spi *spiDriver;
};

#endif	/* NRF24L01P_H */

