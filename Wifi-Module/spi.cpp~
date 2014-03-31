/* 
 * File:   spi.cpp
 * Author: degrigis
 * 
 * Created on January 9, 2014, 4:57 PM
 */

#include "spi.h"
#include <miosix.h>
#include <miosix/kernel/scheduler/scheduler.h>


spi::spi() {
    
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; //active clock to SPI2 ( pg 181 programmer manual)
    SPI2->CR1 = SPI_CR1_SSM  | // software slave management 
                SPI_CR1_SSI  | // internal slave select
                SPI_CR1_MSTR | // setta come master configuration
                SPI_CR1_BR_2 | // setta il bound rate SPI_CR1_BR_2 ((uint16_t)0x0020)
                SPI_CR1_SPE;   // accende la periferica
    //il control register C1 dell'SPI ora è: 0000 0011 0110 0100 ( vedi programmer manual pg 896 )
    
    
}

spi::spi(const spi& orig) {
}

spi::~spi() {
}

/**
 * Send data over spi
 *
 * When RXNE is set indicates that there are valid received data in the buffer RX.
 * It is reset when SPI_DR register is read.
 * 
 *  SPI_SR_RXNE = Receive buffer Not Empty ( programmer manual pg 900 )
 *  if RXNE=0 the buffer is empty. 
 * 
 *  A write on data register (DR) writes on the TX buffer,
 *  A read from data register returns the value in the RX buffer
 * 
 * @param data_byte data to be send through SPI 
 */
void spi::send(int data_byte)
{
    SPI2->DR = data_byte; // setto il data register dell'SPI con il dato arrivato
    while((SPI2->SR & SPI_SR_RXNE)==0); // waiting 
    
    data_byte = SPI2->DR; // come da commento questo fa una dummy read che azzera il bit RXNE
}

/**
 * Shortcut to read data from SPI by a dummy write on SPI2->DR
 * @return the data from spi 
 */
int spi::receive()
{
    SPI2->DR=0;
    while((SPI2->SR & SPI_SR_RXNE)==0);
    return SPI2->DR;
}



