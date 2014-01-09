/* 
 * File:   nRF24L01P.cpp
 * Author: r0rshark
 * 
 * Created on 3 gennaio 2014, 17.22
 */

#include "nRF24L01P.h"
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



nRF24L01P::nRF24L01P() {
}

nRF24L01P::nRF24L01P(const nRF24L01P& orig) {
}

nRF24L01P::~nRF24L01P() {
    MISO::mode(Mode::ALTERNATE);
    MISO::alternateFunction(5);
    MOSI::mode(Mode::ALTERNATE); 
    MOSI::alternateFunction(5);
    IRQ::mode(Mode::INPUT);
    SCK::mode(Mode::ALTERNATE);
    SCK::alternateFunction(5);
    CS::mode(Mode::OUTPUT);
    CS::high();
    greenLed::mode(Mode::OUTPUT);
    greenLed::high(); /*test*/
    
}

void nRF24L01P::init() {
    spi = new spi_driver();
}
void nRF24L01P::transmit(int num_passi){

}

int nRF24L01P::recive(){
    return 0;

}

