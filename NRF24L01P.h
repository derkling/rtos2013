/* 
 * File:   NRF24L01P.h
 * Author: degrigis
 *
 * Created on January 9, 2014, 7:38 PM
 */

#ifndef NRF24L01P_H
#define	NRF24L01P_H

class NRF24L01P {
public:
    NRF24L01P();
    NRF24L01P(const NRF24L01P& orig);
    virtual ~NRF24L01P();
    writeRegister();
    readRegister();
    readStatusRegister();
    powerUp();
    powerDown();
    setReceiveMode();
    setTransmitMode();
    returnStandByI();
    sendDataToTx();
    receiveDataFromRx();
    readRPD();
    configureInterrupt();
private:

};

#endif	/* NRF24L01P_H */

