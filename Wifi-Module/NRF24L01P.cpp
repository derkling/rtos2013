/* 
 * File:   NRF24L01P.cpp
 * Author: degrigis
 * 
 * Created on January 9, 2014, 7:38 PM
 * 
 * 
 * BASIC USAGE
 * 
 * When the wifi-module is correctly initialized a blueLed is on in the STM32
 * 
 * The data to send are in a global variable called out_data
 * The data received from other board are stored in a global variable called in_data 
 * 
 */

#include "NRF24L01P.h"
#include <miosix.h>
#include <miosix/kernel/scheduler/scheduler.h>
#include <stdio.h>
#include <stdlib.h>
#include "spi.h"


using namespace miosix;

typedef Gpio <GPIOB_BASE,15> mosi;
typedef Gpio <GPIOB_BASE,14> miso;
typedef Gpio <GPIOB_BASE,13> sck;
typedef Gpio <GPIOB_BASE,12> cs;
typedef Gpio <GPIOB_BASE,11> ce;
typedef Gpio <GPIOA_BASE,1> irq;

//DEBUG
typedef Gpio <GPIOD_BASE, 15> bluLed;

typedef Gpio <GPIOD_BASE, 14> redLed;

typedef Gpio <GPIOD_BASE, 13> orangeLed;



// thanks to this enum I can check the status of transceiver via software
// unless read the register.
typedef enum {
_NRF24L01P_MODE_UNDEFINED,
_NRF24L01P_MODE_POWER_DOWN,
_NRF24L01P_MODE_STANDBY,
_NRF24L01P_MODE_RX_MODE,
_NRF24L01P_MODE_TX_MODE,
} NRF24L01P_MODE;


int nrf24l01p_mode;


NRF24L01P::NRF24L01P() {
   bluLed::mode(Mode::OUTPUT);
   redLed::mode(Mode::OUTPUT);
   orangeLed::mode(Mode::OUTPUT);
   spiDriver = new spi();
   mosi::mode(Mode::ALTERNATE);
   mosi::alternateFunction(5); 
   miso::mode(Mode::ALTERNATE);
   miso::alternateFunction(5);
   sck::mode(Mode::ALTERNATE);
   sck::alternateFunction(5);
   cs::mode(Mode::OUTPUT);
   ce::mode(Mode::OUTPUT);
   cs::high();
   ce::low();
   
   nrf24l01p_mode = _NRF24L01P_MODE_UNDEFINED; // starts with an undefined state
}

NRF24L01P::NRF24L01P(const NRF24L01P& orig) {   
}

NRF24L01P::~NRF24L01P() {
}

/**
 * This function put the transceiver in stand by I mode ( see charts ) 
 * Have to improve this function with a customizable power_up settings 
 */
void NRF24L01P::powerUp()
{
   int current_config , new_config;
   current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
   new_config = current_config | _NRF24L01P_CONFIG_PWR_UP; // put to 1 the PWR_UP bit in CONFIG register
   
   writeRegister(_NRF24L01P_REG_CONFIG, new_config );
   
   //now have to wait to worst time from power_down to standby ( charts for info )
   usleep(_NRF24L01P_TIMING_Tpd2stby_us);
   
   nrf24l01p_mode = _NRF24L01P_MODE_STANDBY; // now we are in standby mode 
   bluLed::high();
}

/**
 * Configure the TX-Address 
 * @param number
 */
void NRF24L01P::configureTxAddress(int number)
{
    int n_bit = number-2; // see register means
    
    writeRegister(_NRF24L01P_REG_SETUP_AW, n_bit );
    
}

/**
 * Disable all auto-ack on all pipe 
 */
void NRF24L01P::disableAllAutoAck()
{
    this->writeRegister(1,0); //disable all the auto-ack 
}

/**
 * Set the size of the static payload
 * @param size
 */
void NRF24L01P::setStaticPayloadSize(int size)
{
    if(size <1 || size >32)
      {
        printf("[ERROR_MESSAGE]: Payload size must be integer between 1 and 32\n");
      }
    else
        this->writeRegister(17,size); // set bytes of static payload
}

/**
 * PowerDown the module
 */
void NRF24L01P::powerDown()
{
 
int current_config , new_config;
current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
new_config = current_config & _NRF24L01P_CONFIG_NO_PWR_UP; // put to 0 the PWR_UP bit in CONFIG register

writeRegister(_NRF24L01P_REG_CONFIG, new_config );
nrf24l01p_mode = _NRF24L01P_MODE_POWER_DOWN; 

}

/**
 * Reset default value to all registers of module
 */
void NRF24L01P::resetModule()
{
    redLed::high();
    ce::low();
            
    this->flushRx();
    this->flushTx();
    
    writeRegister(_NRF24L01P_REG_CONFIG, 8); //reset config_register to 00001000
    writeRegister(_NRF24L01P_REG_EN_AA, 63);
    writeRegister(_NRF24L01P_REG_EN_RXADDR, 3);
    writeRegister(_NRF24L01P_REG_SETUP_AW,3);
    writeRegister(_NRF24L01P_REG_SETUP_RETR,3);
    writeRegister(_NRF24L01P_REG_RF_CH, 2);
    writeRegister(_NRF24L01P_REG_RF_SETUP,14);
    writeRegister(_NRF24L01P_REG_STATUS,14);
    writeRegister(_NRF24L01P_REG_OBSERVE_TX, 0);
    writeRegister(_NRF24L01P_REG_RPD, 0);
    writeRegister(_NRF24L01P_REG_RX_ADDR_P0, 996028180455);
    writeRegister(_NRF24L01P_REG_RX_ADDR_P1, 836491199170);
    writeRegister(_NRF24L01P_REG_RX_ADDR_P2, 195);
    writeRegister(_NRF24L01P_REG_RX_ADDR_P3, 196);
    writeRegister(_NRF24L01P_REG_RX_ADDR_P4, 197);
    writeRegister(_NRF24L01P_REG_RX_ADDR_P5, 198);
    writeRegister(_NRF24L01P_REG_TX_ADDR, 996028180455 );
    writeRegister(_NRF24L01P_REG_RX_PW_P0,0);
    writeRegister(_NRF24L01P_REG_RX_PW_P1,0);
    writeRegister(_NRF24L01P_REG_RX_PW_P2,0);
    writeRegister(_NRF24L01P_REG_RX_PW_P3,0);
    writeRegister(_NRF24L01P_REG_RX_PW_P4,0);
    writeRegister(_NRF24L01P_REG_RX_PW_P5,0);
    writeRegister( _NRF24L01P_REG_FIFO_STATUS,17);
    writeRegister(_NRF24L01P_REG_DYNPD, 0);
    writeRegister(_NRF24L01P_REG_FEATURE,0);
 
    redLed::low();
    bluLed::high();

}

/**
 * Used to return in standby-i mode from the ReceiveMode
 * The return from TransmitMode to sandby-i mode is automatically performed by Enhanced ShockBurst 
 */
void NRF24L01P::returnStandByI()
{
    ce::low();
    nrf24l01p_mode = _NRF24L01P_MODE_STANDBY;
}

/**
 * Used to mask the wanted irq 
 * 000 no masking
 * 010 mask tx_ds
 * 001 mask max_rt
 * 100 mask rx_dr ( pointless ) 
 * ecc...
 * @param w
 */
void NRF24L01P::maskIrq(int w)
{                
   int current_config , new_config; 

    switch(w)
    {
        case 2: {  
                 current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
                 new_config = current_config |_NRF24L01P_CONFIG_MASK_TX_DS  ; // mask the TX_DS 
                 writeRegister(_NRF24L01P_REG_CONFIG, new_config); 
                 };break;
        case 4: {  
                 current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
                 new_config = current_config |_NRF24L01P_CONFIG_MASK_RX_DR  ; // mask the RX_DR 
                 writeRegister(_NRF24L01P_REG_CONFIG, new_config);
                 };break;
        case 1: {
                 current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
                 new_config = current_config |_NRF24L01P_CONFIG_NO_MASK_MAX_RT ; // mask the MAX_RT
                 writeRegister(_NRF24L01P_REG_CONFIG, new_config);
                };break;
        default: {
                 printf("Wrong masking code\n");
                 };break;
    }
}
/**
 * Remember that the RF channel is set according to formula: F0 = 2400 + RF_CH [ MHZ ]
 * the frequency are from 2,4 GHZ to 2,525GHZ, so the offset is between 0 and 125
 */
void NRF24L01P::setRfChannel(int offset)
{
    if(offset < 0 || offset >125)
        printf("Wrong offset\n");
    writeRegister(_NRF24L01P_REG_RF_CH,offset );

}

/**
 * AirDataRate    air_rate
 *   250kbps         38      better receiver sensitivity 
 *    1Mbps           6
 *    2Mbps          14      lower power consumption and low collision
 *   
 */
void NRF24L01P::setAirDataRate(int air_rate)
{
    if(air_rate == 38 || air_rate == 6 || air_rate == 14) // must be a valid air data rate 
        writeRegister(_NRF24L01P_REG_RF_SETUP , air_rate);
    else
        printf("Wrong air_rate value\n");
}


/**
 In order to call this function you have to call power_up, PWR_UP MUST be 1! 
 */
void NRF24L01P::setReceiveMode()
{
   int current_config , new_config;

   current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
   
   if( nrf24l01p_mode != _NRF24L01P_MODE_STANDBY ) // security control, if somebody call this function before settings transceiver
     {
       fprintf(stderr, "You have to call powerUp first\n");
       return;
     }
   
   new_config = current_config | _NRF24L01P_CONFIG_PRIM_RX ; // put to 1 the PRIM_RX bit in CONFIG register 
   
   writeRegister(_NRF24L01P_REG_CONFIG, new_config);
   ce::high();
   
   usleep(_NRF24L01P_TIMING_Tstby2a_us);
   
   usleep(40); // sleeping to activate RPD
   
   
   nrf24l01p_mode = _NRF24L01P_MODE_RX_MODE;
   
}

/**
 * Function that transmit the data and then return in receive mode 
 * @param data: data is a char pointer to the data to send, if it is a integer it scan byte per byte the integer
 */
void NRF24L01P::TrasmitData(char *data , int dim)
{
    int i;
    ce::low();
    
    if(dim > _NRF24L01P_TX_FIFO_SIZE  ) dim = _NRF24L01P_TX_FIFO_SIZE; //check the size of data
    
    cs::low(); //---SPI START! 
    
    spiDriver->send(_NRF24L01P_SPI_CMD_WR_TX_PAYLOAD ); // request to write on the TX_BUFFER a payload
    
    for(i=0;i<dim;i++) 
        {
         spiDriver->send(*data++);
        }
    
    cs::high(); //---SPI END!
           
    this->setTransmitMode(); //switch to transmission mode
    
    ce::high();
    usleep(_NRF24L01P_TIMING_Thce_us); //ce high for more than 10us
    ce::low(); 
        
    usleep(30000); //wait for transmission
    
    this->resetTXirq(); //reset tx_ds irq bit 
    ce::low(); //return to stand-by
    usleep(_NRF24L01P_TIMING_Tpece2csn_us);
    this->setReceiveMode(); //return to receive mode

}

void NRF24L01P::flushTx()
{
  cs::low();
  spiDriver->send( _NRF24L01P_SPI_CMD_FLUSH_TX  );  //flush tx buffer
  cs::high();
}

void NRF24L01P::flushRx()
{
  cs::low();
  spiDriver->send( _NRF24L01P_SPI_CMD_FLUSH_RX  );  //flush rx buffer
  cs::high();
}

/**
 * used to take data from RX_FIFO_BUFFER through SPI 
 * @return 
 */
int NRF24L01P::receiveDataFromRx()
{   
    int content;
    cs::low();
    spiDriver->send(_NRF24L01P_SPI_CMD_RD_RX_PAYLOAD);
    content = spiDriver->receive();
    cs::high();
    
    return content;
}

/**
 * This function is used to read the RPD bit that 
 * tells if a valid RF signal >-64dbm is detected. 
 * This means that a NRF24L01p is near us, so we can send our payload.
 * @return 
 */
int NRF24L01P::readRPD()
{
   if( nrf24l01p_mode != _NRF24L01P_MODE_RX_MODE ) // security check
     {
       fprintf(stderr, "Failed to read RPD, must be in RX_MODE\n");
       return 0;
     }
   
   return readRegister(_NRF24L01P_REG_RPD);
    
}



/**
 * Remember to call this function after call to returnStandBy-I
 * Before call this function a packet in TX must be present, so I check if TX is empty or not 
 */
void NRF24L01P::setTransmitMode()
{
  this->returnStandByI();
  int current_config , new_config;
  current_config = readRegister(_NRF24L01P_REG_CONFIG);
  new_config = current_config & _NRF24L01P_CONFIG_NO_PRIM_RX; // actually, this is 1111 1110 and preserve previous config, deleting the PRIM_RX
  
  writeRegister(_NRF24L01P_REG_CONFIG , new_config );
  
  nrf24l01p_mode = _NRF24L01P_MODE_TX_MODE;
  
  //Actually starts the transmission packet flow charts
}

void NRF24L01P::writeRegister(int regAddress, int regData)
{
    this->returnStandByI();
    int request;
    request = regAddress | _NRF24L01P_SPI_W_REGISTER;
   
    cs::low();
    
    spiDriver->send(request);
    spiDriver->send(regData); 
    
    cs::high();
}

/**
 * This function read a general register on the NRF24L01P
 * 
 * @param regAddress: the address of the register, see above for the define
 * @return : the value in hex of the register
 * 
 */
int NRF24L01P::readRegister(int regAddress) 
{
    // request = regAddress; tanto questo è in | bit a bit con 0x00 ( READ ) e in & bit a bit con una MASK 0x1f
    int value;
    
    //every spi commands must starts with a high to low cs signal
    cs::low();
    
    spiDriver->send(regAddress);
    value = spiDriver->receive();
    
    cs::high();
    
    return value;
}

void NRF24L01P::showInternal()
{   
    int result;
    
    result = this->readRegister(0);
    printf("<<CONFIG REGISTER>> è: %d\n" , result);
    
    result = this->readRegister(1);
    printf("EN_AA REGISTER è: %d\n" , result);
    
    result = this->readRegister(2);
    printf("EN_RXADDR REGISTER è: %d\n" , result);
    
    result = this->readRegister(3);
    printf("SETUP_AW REGISTER è: %d\n" , result);
    
    result = this->readRegister(4);
    printf("SETUP_RETR REGISTER è: %d\n" , result);
    
    result = this->readRegister(5);
    printf("RF_CH REGISTER è: %d\n" , result);
    
    result = this->readRegister(6);
    printf("RF_SETUP REGISTER è: %d\n" , result);
    
    result = this->readRegister(7);
    printf("<<STATUS REGISTER>> è: %d\n" , result);
    
    result = this->readRegister(8);
    printf("OBSERVE_TX REGISTER è: %d\n" , result);
    
    result = this->readRegister(9);
    printf("<<RPD REGISTER>> è: %d\n" , result);
    
    result = this->readRegister(10);
    printf("RX_ADDR_P0 REGISTER è: %d\n" , result);
    
     result = this->readRegister(11);
    printf("RX_ADDR_P1 REGISTER è: %d\n" , result);
    
     result = this->readRegister(12);
    printf("RX_ADDR_P2 REGISTER è: %d\n" , result);
    
     result = this->readRegister(13);
    printf("RX_ADDR_P3 REGISTER è: %d\n" , result);
    
     result = this->readRegister(14);
    printf("RX_ADDR_P4 REGISTER è: %d\n" , result);
    
     result = this->readRegister(15);
    printf("RX_ADDR_P5 REGISTER è: %d\n" , result);
            
     result = this->readRegister(16);
    printf("TX_ADDR REGISTER è: %d\n" , result);
    
    result = this->readRegister(17);
    printf("RX_PW_P0 REGISTER è: %d\n" , result);
    
    result = this->readRegister(18);
    printf("RX_PW_P1 REGISTER è: %d\n" , result);
    
    result = this->readRegister(19);
    printf("RX_PW_P2 REGISTER è: %d\n" , result);
    
    result = this->readRegister(20);
    printf("RX_PW_P3 REGISTER è: %d\n" , result);
   
    result = this->readRegister(21);
    printf("RX_PW_P4 REGISTER è: %d\n" , result);
    
    result = this->readRegister(22);
    printf("RX_PW_P5 REGISTER è: %d\n" , result);
    
    result = this->readRegister(23);
    printf("<<FIFO_STATUS>> REGISTER è: %d\n" , result);
    
    result = this->readRegister(29);
    printf("FEATURE REGISTER è: %d\n" , result);

}
/**
 * 
 * the status register is passed via miso after a dummy write on mosi
 * @return : the value in hex of status register of NRF24L01P
 */
int NRF24L01P::readStatusRegister()
{
    int value;
    cs::low();
    value=spiDriver->receive(); // in this there is a dummy write of 0 on MOSI
    cs::high();
    return value; 

}

/**
 * Write '1' to the RX_DR bit to clear it
 */
void NRF24L01P::resetRXirq()
{
    int current_config = this->readStatusRegister();
    int new_config = current_config | _NRF24L01P_STATUS_RX_DR; // reset RX_DR for next IRQ
    writeRegister(_NRF24L01P_REG_STATUS,  new_config );
}

/**
 * Write '1' to the TX_DS bit to clear it
 */
void NRF24L01P::resetTXirq()
{
 int current_config = this->readStatusRegister();
 int new_config = current_config | _NRF24L01P_STATUS_TX_DS; // reset TX_DS for next IRQ
 writeRegister(_NRF24L01P_REG_STATUS,  new_config );
}

void NRF24L01P::configureInterrupt()
{
    irq::mode(Mode::INPUT);
    EXTI->IMR |= EXTI_IMR_MR1;
    EXTI->RTSR |= EXTI_RTSR_TR1;
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_SetPriority(EXTI1_IRQn,15); //Low priority
    
}






