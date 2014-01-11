/* 
 * File:   nRF24L01P.cpp
 * Author: r0rshark
 * 
 * Created on 3 gennaio 2014, 17.22
 */

#include "nRF24L01P.h"
#include <cstdio>
#include "miosix.h"

//NRF24L01P Macro 
//pipe number for multiceiver
#define NRF24L01P_PIPE_NO_0     0
#define NRF24L01P_PIPE_NO_1     1
#define NRF24L01P_PIPE_NO_2     2
#define NRF24L01P_PIPE_NO_3     3
#define NRF24L01P_PIPE_NO_4     4
#define NRF24L01P_PIPE_NO_5     5

//size buffers
#define NRF24L01P_RX_BUFFER_SIZE       32


//Command
#define NRF24L01P_CMD_RD_REG     0x00
#define NRF24L01P_CMD_WT_REG     0x20
#define NRF24L01P_CMD_NOP        0xff
#define NRF24L01P_R_RX_PAY       0x61

//bitmask and register address
#define NRF24LO1P_REG_ADDR_BITMASK    0x1f
#define NRF24L01P_REG_CONF            0x00
#define NRF24L01P_REG_STATUS          0x07


//register config
#define NRF24L01P_PRIM_RX        (1<<0)         //set to recive 
#define NRF24L01P_PWR_UP         (1<<1)         //set to power up
//register status
#define NRF24L01P_STATUS_DR_RX   (1<<6)         //set if data register full write 1 to clear
#define NRF24L01P_STATUS_RX_P_NO (0x7<<1)       //set with number of pipe which is full      

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
    CE::high();
    
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
    if (CE::value()==0){
        CE_enable();
    }
    mode = NRF24L01P_RX_MODE;
   
}

void nRF24L01P::set_transmit_mode(){
    if (mode==NRF24L01P_POWER_DOWN_MODE){
        power_up();
    }
    int cur_config = get_register(NRF24L01P_REG_CONF);
    cur_config &= ~NRF24L01P_PRIM_RX;
    set_register(NRF24L01P_REG_CONF,cur_config);
    if (CE::value()==0){
        CE_enable();
    }
    mode = NRF24L01P_TX_MODE;
    
}


void nRF24L01P::transmit(int num_passi){
    
}

int nRF24L01P::receive(int pipe,char *data,int count){
    if(mode!=NRF24L01P_RX_MODE){
        printf("Before receive set up in recieve_mode\n");
        return -1;
    }
    if (pipe<NRF24L01P_PIPE_NO_0 || pipe>NRF24L01P_PIPE_NO_5){
        printf("Error number of pipe must be between 0 and 5 not %d\n",pipe);
        return -1;
    }
    if (count<=0) {
        return 0;
    }
    if (count>NRF24L01P_RX_BUFFER_SIZE){
        count= NRF24L01P_RX_BUFFER_SIZE;
    }
    if(packet_in_pipe(pipe)){
        //NB----I skip the phase of check the lenght of the packet
        
        CS::low();
        spi->spi_write(NRF24L01P_R_RX_PAY);
        
        for(int i=0;i<count;i++){
            *data = spi->spi_Receive();
            data++;
        }
        CS::low();
        //clear RX_DR status bit
        set_register(NRF24L01P_REG_STATUS,NRF24L01P_STATUS_DR_RX);
        return count;
    }
    else{
        printf("Pipe chosen is empty\n");
        return 0;
    }
    return 0;
}

void nRF24L01P::CE_enable(){
    CE::high();
    usleep(NRF24L01P_TPECE2CSN);        //sleep to apply ce value change
}

void nRF24L01P::CE_disable(){
    CE::low();
}

/**
 * function allowes to set a register to a particular value
 * @param addr_registro address of the register
 * @param data_registro data to set the register
 */
void nRF24L01P::set_register(int addr_registro,int data_registro){
        int old_ce =CE::value();  //save the CE value    
        CE_disable(); //in order to change value of register the module has to be in StandBy1 mode
        CS::low();
        spi->spi_write(NRF24L01P_CMD_WT_REG |(addr_registro & NRF24LO1P_REG_ADDR_BITMASK)); //command to write the at correct address of register
        spi->spi_write(data_registro & 0xFF);    //data used to set the register
        CS::high();
        old_ce ? CE_enable():CE_disable();                  //restore old ce value
        

}

int  nRF24L01P::get_register(int registro){
    int command = NRF24L01P_CMD_RD_REG | (registro & NRF24LO1P_REG_ADDR_BITMASK);
    int result;
    CS::low();
    spi->spi_write(command);   
    result = spi->spi_Receive();
    CS::high();
    return result;   
}

bool nRF24L01P::packet_in_pipe(int pipe){
    if ((pipe<NRF24L01P_PIPE_NO_0) || (pipe> NRF24L01P_PIPE_NO_5)){
        return false;
    }
    int status=get_status_register();
    //& is bitwise (it returns 01001100) && is and (return 0 or 1))
    if((status & NRF24L01P_STATUS_DR_RX)&&((status & NRF24L01P_STATUS_RX_P_NO)>>1)==(pipe & 0x7)){
        return true;
    }
    return false;
}

/**
 * Function to get the status register
 * @return status register
 */
int nRF24L01P::get_status_register(){
    CS::low();
    int status = spi->spi_Receive();    //the module send status bit every time is sent a command
    CS::high();
    return status;
}

void nRF24L01P::test(){
    power_down();
    printf("Config register at power down %d\n",get_register(NRF24L01P_REG_CONF));
    power_up();
    printf("Config register at power up %d\n",get_register(NRF24L01P_REG_CONF));
    set_transmit_mode();
    printf("Config register at transmit %d\n",get_register(NRF24L01P_REG_CONF));
    set_receive_mode();
    printf("Config register at receive %d\n",get_register(NRF24L01P_REG_CONF));
    char *data;
    usleep(3000000);
    int received_lenght_data = receive(0,data,1);
    printf("receive result: %d\n",received_lenght_data);
    printf("Status register %d\n",get_status_register());
}




