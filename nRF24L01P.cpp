/* 
 * File:   nRF24L01P.cpp
 * Author: r0rshark
 * 
 * Created on 3 gennaio 2014, 17.22
 */

#include "nRF24L01P.h"
#include "interfaces-impl/gpio_impl.h"

//module define
#define NRF24L01P_CMD_RD_REG 0x00
#define NRF24LO1P_RG_BITMASK 0x1f



nRF24L01P::nRF24L01P() {
}

nRF24L01P::nRF24L01P(const nRF24L01P& orig) {
}

nRF24L01P::~nRF24L01P() {
}

void nRF24L01P::init() {
    spi = new spi_driver();
}
void nRF24L01P::transmit(int num_passi){

}

int nRF24L01P::receive(){
    return 0;
}

int  nRF24L01P::get_register(int registro){
    int command = NRF24L01P_CMD_RD_REG | (registro & NRF24LO1P_RG_BITMASK);
    int result;
    CS.low();
    spi->spi_write(command);   
    result = spi->spi_Receive();
    CS.high();
    return result;
    
    
    
}


