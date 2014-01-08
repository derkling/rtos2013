/* 
 * File:   spi_driver.cpp
 * Author: r0rshark
 * 
 * Created on 3 gennaio 2014, 17.10
 */

#include "spi_driver.h"
#include <miosix.h>
#include <miosix/kernel/scheduler/scheduler.h>

using namespace miosix;

/*Spi Gpio*/
typedef Gpio<GPIOB_BASE,11> CE;
typedef Gpio<GPIOB_BASE,12> CS;
typedef Gpio<GPIOB_BASE,13> SCK;
typedef Gpio<GPIOB_BASE,14> MISO;
typedef Gpio<GPIOB_BASE,15> MOSI;
typedef Gpio<GPIOA_BASE,1> IRQ;

/*Led Gpio*/
typedef Gpio<GPIOD_BASE,12> greenLed;



spi_driver::spi_driver() {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; /*attivo il clock*/
    
    MISO::mode(Mode::ALTERNATE);
    MISO::alternateFunction(5);
    MOSI::mode(Mode::ALTERNATE); 
    MOSI::alternateFunction(5);
    IRQ::mode(Mode::INPUT);
    SCK::mode(Mode::ALTERNATE);
    SCK::alternateFunction(5);
    CS::mode(Mode::OUTPUT);
    CS::high();
    
    //guida a pagina 861
    // SPI control register 1 

    SPI2->CR1 = SPI_CR1_BR_2 |                       //1)imposto il baud rate andando a settere il secondo bit di BR (100 non contemplato nel reference manual page 897))
                                                                                //2)dovrei settare CPOL e CPHA ma non ho capito cosa fanno
                                                                                    //3)dff impostato a 0 -> 8 bit di frame
                                                                                    //4)LSBFIRST a 0 così trasmetto in MSB format  bit piu significativo per primo
                        SPI_CR1_SSM |                                    //5)Slave Select software abilitato
                        SPI_CR1_SSI |                                      //6)Internal slave select  DEVO settare anche SSOE non so...
                        SPI_CR1_MSTR  |                                  //7)Devo per forza settarlin 
                        SPI_CR1_SPE;
    
   
    greenLed::mode(Mode::OUTPUT);
    greenLed::high(); /*test*/
}

spi_driver::spi_driver(const spi_driver& orig) {
}

spi_driver::~spi_driver() {
}

void spi_driver::spi_write(unsigned char byte){
    SPI2->DR = byte;
    while((SPI2->SR & SPI_SR_RXNE)==0); 
    byte=SPI2->DR;
   
}

unsigned char spi_driver::spi_Receive(){
    
  return 'a';  
}

