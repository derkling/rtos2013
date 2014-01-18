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
//rf register 
 
#define NRF24L01P_RF_HIGH_BIT       (1 << 3)
#define NRF24L01P_RF_LOW_BIT        (1 << 5)
#define NRF24L01P_RF_MASK           (NRF24L01P_RF_LOW_BIT| NRF24L01P_RF_HIGH_BIT)
#define NRF24L01P_RF_250KBPS        (NRF24L01P_RF_LOW_BIT)
#define NRF24L01P_RF_1MBPS          (0)
#define NRF24L01P_RF_2MBPS          (NRF24L01P_RF_HIGH_BIT)

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
#define NRF24L01P_CMD_RD_REG                    0x00
#define NRF24L01P_CMD_WT_REG                    0x20
#define NRF24L01P_CMD_WR_TX_PAYLOAD             0xa0
#define NRF24L01P_CMD_NOP        0xff
#define NRF24L01P_R_RX_PAY       0x61
#define NRF24L01P_SPI_CMD_FLUSH_TX          0xe1

//bitmask and register address
#define NRF24LO1P_REG_ADDR_BITMASK              0x1f
#define NRF24L01P_REG_CONF                      0x00
#define NRF24L01P_REG_STATUS                    0x07
#define NRF24L01P_REG_RF_CH                     0x05
#define NRF24L01P_REG_RF_SETUP                  0x06
#define NRF24L01P_REG_AA                        0x01
#define NRF24L01P_REG_SETUP_AW                  0x03
#define NRF24L01P_REG_TX_ADDR                   0x10
#define NRF24L01P_REG_RX_ADDR_P0            0x0a

#define NRF24L01P_PRIM_RX                       (1<<0)
#define NRF24L01P_PWR_UP                        (1<<1)
#define NRF24L01P_STATUS_TX_DS                  (1<<5)
#define NRF24L01P_STATUS_MAX_RT                 (1<<4)
#define NRF24L01P_STATUS_RX_DR                  (1<<6)
#define NRF24L01P_RF_SETUP_RF_PWR_MASK          (0x3<<1)
#define NRF24L01P_RF_SETUP_RF_DR_MASK           (40<<0)
#define NRF24L01P_RF_SETUP_PWR_0DBM             (0x3<<1)
#define NRF24L01P_RF_SETUP_PWR_MINUS_6DBM       (0x2<<1)
#define NRF24L01P_RF_SETUP_PWR_MINUS_12DBM      (0x1<<1)
#define NRF24L01P_RF_SETUP_PWR_MINUS_18DBM      (0x0<<1)
#define NRF24L01P_RF_DR_250KBPS                 (1<<5)
#define NRF24L01P_RF_DR_1MBPS                   (0)
#define NRF24L01P_RF_DR_2MBPS                   (1<<3)
#define NRF24L01P_STATUS_DR_RX                  (1<<6)         //set if data register full write 1 to clear
#define NRF24L01P_STATUS_RX_P_NO                (0x7<<1) 


//time
#define NRF24L01P_TPD2STBY                      2000  //2mS
#define NRF24L01P_TPECE2CSN                     4  //4uS

//size
#define NRF24L01P_TX_FIFO_SIZE                  32
#define NRF24L01P_MIN_RF_FREQUENCY              2400
#define NRF24L01P_MAX_RF_FREQUENCY              2525
#define NRF24L01P_TX_PWR_ZERO_DB                 0
#define NRF24L01P_TX_PWR_MINUS_6_DB             -6
#define NRF24L01P_TX_PWR_MINUS_12_DB            -12
#define NRF24L01P_TX_PWR_MINUS_18_DB            -18
#define NRF24L01P_DATARATE_250KBPS              250
#define NRF24L01P_DATARATE_1MBPS                1000
#define NRF24L01P_DATARATE_2MBPS                2000
#define NRF24L01P_EN_AA_NONE                   0



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
    setup_Gpio();
    //power_down();
    set_register(NRF24L01P_REG_STATUS, NRF24L01P_STATUS_TX_DS | NRF24L01P_STATUS_MAX_RT |
                                NRF24L01P_STATUS_RX_DR);/*clear every pending interrupt bits*/
    
       // set_frequency(2450);
     // set_power_output(-12);
     // set_air_data_rate(1000);    
     set_register(NRF24L01P_REG_AA, NRF24L01P_EN_AA_NONE);// deactivate wait for ack*/
    
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
    set_tx_address(3);
    set_register(NRF24L01P_REG_TX_ADDR , 5);
    set_register(NRF24L01P_REG_RX_ADDR_P0 ,5);
    mode=NRF24L01P_STANDBY_MODE;
    flushTx();
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
    printf("Inizio transmit \n");
    if (mode==NRF24L01P_POWER_DOWN_MODE){
        power_up();
    }
    int cur_config = get_register(NRF24L01P_REG_CONF);
    cur_config &= ~NRF24L01P_PRIM_RX;
    set_register(NRF24L01P_REG_CONF,cur_config);
    
        CE_enable();
    
    mode = NRF24L01P_TX_MODE;
    printf("Fine transmit \n");
    
}
/**
 * function allowes to transmit a data with the nRF24L01P module
 * @param count dimension of data
 * @param data data to send
 * @return number of bits sent
 */
int nRF24L01P::transmit(int count, char* data){
    int old_ce = CE::value();
    if( count < 0)
        return 0;
    if( count > NRF24L01P_TX_FIFO_SIZE)
        count = NRF24L01P_TX_FIFO_SIZE;
    CE_disable();
    set_register(NRF24L01P_REG_STATUS, NRF24L01P_STATUS_TX_DS); /*clear bit interrupt data sent tx fifo*/
    CS::low();
    spi->spi_write(NRF24L01P_CMD_WR_TX_PAYLOAD); //command to start write from payload TX
    /*for( int i=0; i<count; i++){
        printf("char %c\n",*data);
        spi->spi_write(*data++);
    }*/
    spi->spi_write(12);
    CS::high();
    int old_mode = mode;
    set_transmit_mode();
    CE_enable();
    CE_disable();
    printf("Before polling \n");
    printf("Get register status %d\n",get_register_status());
    while( !( get_register_status() & NRF24L01P_STATUS_TX_DS)){
        
    } //polling waiting for transfert complete
    printf("After polling\n");
    set_register(NRF24L01P_REG_STATUS, NRF24L01P_STATUS_TX_DS); /*clear bit data sent tx fifo*/
    if( old_mode == NRF24L01P_RX_MODE){              //reset the state before
        set_receive_mode();
    }
    CE_restore(old_ce);
    return count;
    
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

void nRF24L01P::CE_restore(int old_ce){
    old_ce ? CE::high():CE::low();      //wifi_module.cpp:75:34: error: no matching function for call to 'nRF24L01P::receive()'restore old ce value
    usleep(NRF24L01P_TPECE2CSN);    //sleep to apply ce value change
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
        spi->spi_write(data_registro & NRF24L01P_CMD_NOP);    //data used to set the register
        CS::high();
        CE_restore(old_ce);

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
    printf("Status register in packet_inpipe %d",get_register_status());
    int status=get_register_status();
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
int nRF24L01P::get_register_status(){
    CS::low();
    int status = spi->spi_Receive();    //the module send status bit every time is sent a command
    CS::high();
    return status;
}

void nRF24L01P::test_receive(){
    power_up();
    printf("Config register at power up %d\n",get_register(NRF24L01P_REG_CONF));
    printf("Status register before receive %d\n",get_register_status());
    set_receive_mode();
    printf("Config register at receive %d\n",get_register(NRF24L01P_REG_CONF));
    printf("Status register after receive %d\n",get_register_status());

    printf("default value");
    printf( "nRF24L01+ Frequency    : %d MHz\r\n",  get_frequency() );
   // printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    printf( "nRF24L01+ Data Rate    : %d kbps\r\n", get_air_data_rate());
   // printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );
    char *data;
    while(true){
    usleep(3000000);
    printf("Status register %d\n",get_register_status());
    printf("Config register %d\n",get_register(NRF24L01P_REG_CONF));
    int received_lenght_data = receive(NRF24L01P_PIPE_NO_0,data,1);
    printf("ricevuto da pipe 1 %d\n",receive(1,data,1));
    printf("ho ricevuto %s\n",data);
    printf("receive result: %d\n",received_lenght_data);


    }
}

void nRF24L01P::setup_Gpio(){
    
    MISO::mode(Mode::ALTERNATE);
    MISO::alternateFunction(5);
    MOSI::mode(Mode::ALTERNATE); 
    MOSI::alternateFunction(5);
    IRQ::mode(Mode::INPUT);
    SCK::mode(Mode::ALTERNATE);
    SCK::alternateFunction(5);
    CS::mode(Mode::OUTPUT);
    CS::high();
    CE::mode(Mode::OUTPUT);
    CE::high();
    
}

void nRF24L01P::set_frequency(int frequency){
    printf("Begin set frequency\n");
    if ((frequency < NRF24L01P_MIN_RF_FREQUENCY) | (frequency > NRF24L01P_MAX_RF_FREQUENCY)){
        printf("Error frequency module %d\n",frequency);
        return;
    }
    int channel = (frequency - NRF24L01P_MIN_RF_FREQUENCY) & 0x7F;  /*from manual RF_freq = frequency - NRF24L01P_MIN_RF_FREQUENCY)*/
    set_register(NRF24L01P_REG_RF_CH, channel);
    printf("end set frequency\n");
}
int nRF24L01P::get_frequency(){
    int freq = get_register(NRF24L01P_REG_RF_CH);
    return (freq + NRF24L01P_MIN_RF_FREQUENCY);

}

void nRF24L01P::set_power_output(int power){
    
    int rf_config = get_register(NRF24L01P_REG_RF_SETUP) & ~NRF24L01P_RF_SETUP_RF_PWR_MASK; /*get rf config except for the power bits*/
    printf("Start set power config %d\n", rf_config);
    switch (power){                                     /*set the power*/
        case NRF24L01P_TX_PWR_ZERO_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_0DBM;
            break;
        case NRF24L01P_TX_PWR_MINUS_6_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_MINUS_6DBM;
            break;
        case NRF24L01P_TX_PWR_MINUS_12_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_MINUS_12DBM;
            break;
        case NRF24L01P_TX_PWR_MINUS_18_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_MINUS_18DBM;
            break;
        default:
            printf("Error power module %d\n",power);
            return;
    }
    set_register(NRF24L01P_REG_RF_SETUP, rf_config);    /*set the rf setup register*/
    printf("End set power config %d\n",rf_config);
}

void nRF24L01P::set_air_data_rate(int rate){
    printf("Start set air rate\n");
    int air_config = get_register(NRF24L01P_REG_RF_SETUP) & ~NRF24L01P_RF_SETUP_RF_DR_MASK; /*get rf config except rf_dr_low and rf_dr_high*/
    switch (rate){
        case NRF24L01P_DATARATE_250KBPS:
            air_config |= NRF24L01P_RF_DR_250KBPS;
            break;
        case NRF24L01P_DATARATE_1MBPS:
            air_config |= NRF24L01P_RF_DR_1MBPS;
            break;
        case NRF24L01P_DATARATE_2MBPS:
            air_config |= NRF24L01P_RF_DR_2MBPS;
            break;
        default:
            printf("Error air data rate %d\n",rate);
            return;
    }
    set_register(NRF24L01P_REG_RF_SETUP, air_config);
}

int nRF24L01P::get_air_data_rate() {
 
    int rate = get_register(NRF24L01P_REG_RF_SETUP) & NRF24L01P_RF_SETUP_RF_DR_MASK;
 
    switch ( rate ) {
 
        case NRF24L01P_RF_250KBPS:
            return NRF24L01P_DATARATE_250KBPS;
 
        case NRF24L01P_RF_1MBPS:
            return NRF24L01P_DATARATE_1MBPS;
 
        case NRF24L01P_RF_2MBPS:
            return NRF24L01P_DATARATE_2MBPS;
 
        default:
            printf( "nRF24L01P: Unknown Air Data Rate value %d\r\n", rate );
            return 0;

    }
}

void nRF24L01P::test_transmit(){
    power_down();
    printf("Config register at power down %d\n",get_register(NRF24L01P_REG_CONF));
    power_up();
    printf("Config register at power up %d\n",get_register(NRF24L01P_REG_CONF));
    set_transmit_mode();
    printf("Config register at transmit %d\n",get_register(NRF24L01P_REG_CONF));
    
}

void nRF24L01P::set_tx_address(int number){
    int num_bit = number -2;
    set_register(NRF24L01P_REG_SETUP_AW, num_bit);
}

void nRF24L01P::flushTx()
{
    printf("ORA FLUSH TX\n");
  CS::low();
  spi->spi_write( NRF24L01P_SPI_CMD_FLUSH_TX  );  //svuoto coda TX
  CS::high();
  
}
