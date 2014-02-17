/**
 * File:   spi_driver.cpp
 *  \brief     This is the implementation file for the class spi_driver.
 *  \author    Andrea Corna
 *  \author    Lorenzo Fontana
 *  \version   1.0
 *  \date      03/01/2014
 */

#include "spi_driver.h"
#include <miosix.h>
#include <miosix/kernel/scheduler/scheduler.h>

using namespace miosix;



spi_driver::spi_driver() {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; /*attivo il clock*/
    /*set baud rate with BR bit
     dff bit to 0 → 8 bit frame
     LSBFIRST bit to 0 → MSB */
    SPI2->CR1 = SPI_CR1_BR_2 |                       //1)imposto il baud rate andando a settere il secondo bit di BR (100 non contemplato nel reference manual page 897))
                                                                                //2)dovrei settare CPOL e CPHA ma non ho capito cosa fanno
                                                                                    //3)dff impostato a 0 -> 8 bit di frame
                                                                                    //4)LSBFIRST a 0 così trasmetto in MSB format  bit piu significativo per primo
                        SPI_CR1_SSM |                                    //5)Slave Select software abilitato
                        SPI_CR1_SSI |                                      //6)Internal slave select  DEVO settare anche SSOE non so...
                        SPI_CR1_MSTR  |                                  //7)Devo per forza settarlin 
                        SPI_CR1_SPE;
 
}

spi_driver::spi_driver(const spi_driver& orig) {
}

spi_driver::~spi_driver() {
}

void spi_driver::spi_write(int command){
    SPI2->DR = command;
    while((SPI2->SR & SPI_SR_RXNE)==0); 
    /*dummy read in order to reset RXNE bit*/
    command=SPI2->DR;
}

int  spi_driver::spi_Receive(){
    /*dummy write in order to receive a byte from slave*/
     SPI2->DR = 0xff;             
     while((SPI2->SR & SPI_SR_RXNE)==0);
     return SPI2->DR;  
}

