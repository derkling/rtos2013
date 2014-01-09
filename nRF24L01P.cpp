/* 
 * File:   nRF24L01P.cpp
 * Author: r0rshark
 * 
 * Created on 3 gennaio 2014, 17.22
 */

#include "nRF24L01P.h"
#include <miosix.h>
#include <miosix/kernel/scheduler/scheduler.h>

//NRF24L01P Macro 
//Command
#define NRF24L01P_CMD_RD_REG     0x00
#define NRF24L01P_CMD_WT_REG     0x01

//bitmask and register address
#define NRF24LO1P_REG_ADDR_BITMASK    0x1f
#define NRF24L01P_REG_CONF       0x00

//set data to register
#define NRF24L01P_PRIM_RX        (1<<0)
#define NRF24L01P_PWR_UP         (1<<1)

//time
#define NRF24L01P_TPD2STBY       2000  //2mS
#define NRF24L01P_TPECE2CSN      4     //4uS

typedef enum {
    NRF24L01P_UNKNOWN_MODE,
    NRF24L01P_POWER_DOWN_MODE,
    NRF24L01P_STANDBY_MODE,
    NRF24L01P_RX_MODE,
    NRF24L01P_TX_MODE,
} NRF24L01P_mode;       

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
    spi = new spi_driver();
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

nRF24L01P::nRF24L01P(const nRF24L01P& orig) {
}

nRF24L01P::~nRF24L01P() {
   
}

/**
 * power up the module
 */
void nRF24L01P::power_up() {
    //I get the current config and I add the power up bit then I write it back 
    int current_config = get_register(NRF24L01P_REG_CONF); 
    current_config |= NRF24L01P_PWR_UP;
    set_register(NRF24L01P_REG_CONF,current_config);
    usleep(NRF24L01P_TPD2STBY);
    mode=NRF24L01P_STANDBY_MODE;
}

void nRF24L01P::power_down() {
    int current_config = get_register(NRF24L01P_REG_CONF);
    current_config &= ~NRF24L01P_PWR_UP;
    set_register(NRF24L01P_REG_CONF,current_config);
    usleep(NRF24L01P_TPD2STBY);
    mode = NRF24L01P_POWER_DOWN_MODE;
}

void nRF24L01P::set_receive_mode(){
    
    if (mode==NRF24L01P_POWER_DOWN_MODE){
        power_up();
    }
    int cur_config = get_register(NRF24L01P_REG_CONF);
    cur_config |= NRF24L01P_PRIM_RX;
    set_register(NRF24L01P_REG_CONF,cur_config);
    if (CE.value()==0){
        CE.high();
    }
    usleep(NRF24L01P_TPECE2CSN);
    mode = NRF24L01P_RX_MODE;
   
}

void nRF24L01P::set_transmit_mode(){
    if (mode==NRF24L01P_POWER_DOWN_MODE){
        power_up();
    }
    int cur_config = get_register(NRF24L01P_REG_CONF);
    cur_config &= ~NRF24L01P_PRIM_RX;
    set_register(NRF24L01P_REG_CONF,cur_config);
    if (CE.value()==0){
        CE.high();
    }
    usleep(NRF24L01P_TPECE2CSN);
    mode = NRF24L01P_TX_MODE;
    
}


void nRF24L01P::transmit(int num_passi){

}

int nRF24L01P::receive(){
    return 0;
}
/**
 * function allowes to set a register to a particular value
 * @param addr_registro address of the register
 * @param data_registro data to set the register
 */
void nRF24L01P::set_register(int addr_registro,int data_registro){
        int old_ce =CE.value()  //save the CE value    
        CE.low();               //in order to change value of register the module has to be in StandBy1 mode
        int status = spi.spi_write(NRF24L01P_CMD_WT_REG |(addr_registro & NRF24LO1P_REG_ADDR_BITMASK)); //command to write the at correct address of register
        spi.spi_write(data_registro & 0xff);    //data used to set the register
        CE.value()=old_ce;                      //restore old ce value
        usleep(NRF24L01P_TPECE2CSN);            //sleep to apply ce value change
        

}

int  nRF24L01P::get_register(int registro){
    int command = NRF24L01P_CMD_RD_REG | (registro & NRF24LO1P_REG_ADDR_BITMASK);
    int result;
    CS::low();
    spi.spi_write(command);   
    result = spi.spi_Receive();
    CS::high();
    return result;   
}


