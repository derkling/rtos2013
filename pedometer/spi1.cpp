/* 
 *  \File:     spi1.cpp
 *  \brief     This file contains all the methods to initialize, read and write 
 *             on SPI1 bus.
 *  \details   This file contains 4 public methods:
 *             -SPI() that is the class constructor
 *             -config() that sets up all the CPU register to use the SPI1 to 
 *                       send and receive information from the on board accelerometer
 *             -singleRead() that read the 8 bit sent by the accelerometer
 *             -write() that send single or multiple 8-bit information to the 
 *                      accelerometer
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      08/01/2014
 */
#include <cstdio>
#include "miosix.h"
#include "spi1.h"
#include "spi1_constant.h"
#include "utility.h"

#define ALTERNATE_FUNCTION_SPI1 5
#define SPIg SPI1


using namespace std;
using namespace miosix;

typedef Gpio<GPIOA_BASE, 5> SCK;
typedef Gpio<GPIOA_BASE, 6> MISO;
typedef Gpio<GPIOA_BASE, 7> MOSI;
typedef Gpio<GPIOE_BASE, 3> CS;

Utility* utility_s;

/**
 * \brief class constructor
 */
Spi::Spi(){
    utility_s=Utility::getInstance();
}
/**
 *  \brief This function sets up all the CPU register to enable the SPI1 interface;
 *  @param void
 *  @return void
 */
void Spi::config()
{
    
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    //-------------------------------------------------------------------\\
    /* SPI SCK MOSI MISO pin configuration */
    SCK::mode(Mode::ALTERNATE);
    SCK::alternateFunction(ALTERNATE_FUNCTION_SPI1);
    SCK::speed(Speed::_50MHz);
    
    MISO::mode(Mode::ALTERNATE);
    MISO::alternateFunction(ALTERNATE_FUNCTION_SPI1);
    SCK::speed(Speed::_50MHz);
    
    MOSI::mode(Mode::ALTERNATE);
    MOSI::alternateFunction(ALTERNATE_FUNCTION_SPI1);
    SCK::speed(Speed::_50MHz);
    
    //-------------------------------------------------------------------\\
    
    /*resets the SPIx peripheral registers*/
    RCC->APB2RSTR |= RCC_APB2ENR_SPI1EN;
    RCC->APB2RSTR &= !(RCC_APB2ENR_SPI1EN);
    
    /* Enable the SPI peripheral (Guardare enable per settare)*/
    uint16_t tmpreg = 0;
    tmpreg= (uint16_t)SPIg->CR1;
    tmpreg |= (uint16_t)(SPI_Direction | SPI_Mode | SPI_DataSize | SPI_CPOL |  
                  SPI_CPHA | SPI_NSS |  SPI_BaudRatePrescaler | SPI_FirstBit);
    /* Write to SPIx CR1 */
    SPIg->CR1 = tmpreg;

    /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
    SPIg->I2SCFGR &= (uint16_t)!((uint16_t)SPI_I2SCFGR_I2SMOD);
    /* Write to SPIx CRCPOLY */
    SPIg->CRCPR = SPI_CRCPolynomial;
    
    SPIg->CR1 |= SPI_CR1_SPE;
    //---------------------------------------------------------------------\\
    /* Configure GPIO PIN for Lis Chip select */
    CS::mode(Mode::OUTPUT);
    CS::alternateFunction(ALTERNATE_FUNCTION_SPI1);
    SCK::speed(Speed::_50MHz);
    
    csOff();
}

/*
 *   This function sets the chip select bit 
 */
void Spi::csOn(){
    //spi_typedef_pun->CR1 |= SSM | SPE;
    CS::low();
}

/*
 * This function reset the chip select bit
 */
void Spi::csOff(){
    //spi_typedef_pun->CR1 &= !(SSM | SPE);
    CS::high();
}

/*
 * This function returns the value of Data Register.
 * In Data Register there is stored the last information received on SPI
 */
uint16_t Spi::reciveData(){
    
      return SPIg->DR;
}

/*
 * This function write in Data Register the information that have to be sent 
 * on SPI 
 */
void Spi::sendData(uint8_t data){
    
    SPIg->DR = data;
    
}

/*
 * This function returns 1 if the bus associated to reg parameter is busy
 */
uint16_t Spi::isBusy(int reg){
    
    if((SPIg->SR & (uint16_t)reg) != (uint16_t)RESET){
        return 0;
    }
    return 1;
    
}

/**
 * \brief this function perform a single read on SPI
 *  @param addr the address where the read have to be performed
 *  @return the value of the information at required address
 */
int16_t Spi::singleRead(uint8_t addr){
    
        uint8_t readed = 0;
        addr |= SPI_READ;
    
        /* Transmission start: pull CS low */
        csOn();
        
	/* Send address */
        while(isBusy(SPI_SR_TXE)){}
	sendData(addr);
        
        /* 
         * Dummy read to make sure shift register is empty.
         */
        while(isBusy(SPI_SR_RXNE)){}
        reciveData();

        /* Dummy write */
        while(isBusy(SPI_SR_TXE)){}
        sendData((uint8_t)0x00);
              
        /* Actual read */
        while(isBusy(SPI_SR_RXNE)){}
        readed = reciveData();

	/* Transmission end: pull CS high */
	csOff();

	return readed;
}

/**
 * \brief This function perform the needed write on SPI
 * @param addr the address where information must be written
 * @param buffer the information that have to be sent on SPI
 * @param len the number of write that have to be performed
 * @return -1 if occurs an error 0 else
 */
int Spi::write(uint8_t addr, uint8_t* buffer, uint16_t len){
        
        /*Invalid length*/
	if(len <= 0){
                return -1;
        }
	
	/* set bit 6 of the address for multiple operation */
	if(len > 1){
		addr |= SPI_MULTI_OP;
	}
        
	/* pull CS low to start trasmission */
	csOn();
        
	/* Send address */
        while(isBusy(SPI_SR_TXE)){}
        sendData(addr);
        
        /* Wait data hits slave */ 
        while(isBusy(SPI_SR_RXNE)) {}
        reciveData();
        
        /* Send data */
        while(len--){
                
                while(isBusy(SPI_SR_TXE)){}
                
                sendData(*buffer++);
                        
                while(isBusy(SPI_SR_RXNE)){}
                
                reciveData();
        }

        /* Reset cs at the end of transmission */
        csOff();
        
        /*all went good*/
        return 0;
}
