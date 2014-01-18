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
    void writeRegister(int regAddress, int regData);
    int readRegister(int regAddress);
    int readStatusRegister();
    void powerUp();
    void powerDown();
    void setReceiveMode();
    void setTransmitMode();
    void returnStandByI();
    int receiveDataFromRx();
    int readRPD();
    void configureInterrupt();
    void configureTxAddress(int n_byte);
    void flushTx();
    void flushRx();
    void NoAckOnThisPack();
    void TrasmitData(char* data_pointer , int dim);
    void resetModule();

private:
    spi *spiDriver;
};

#endif	/* NRF24L01P_H */

