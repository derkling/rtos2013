/* 
 * File:   NRF24L01P.cpp
 * Author: degrigis
 * 
 * Created on January 9, 2014, 7:38 PM
 */

#include "NRF24L01P.h"
#include <miosix.h>
#include <miosix/kernel/scheduler/scheduler.h>
using namespace miosix;

/*
 * The following FIFOs are present in nRF24L01+:
 *   TX three level, 32 byte FIFO
 *   RX three level, 32 byte FIFO
 */
#define _NRF24L01P_TX_FIFO_COUNT   3
#define _NRF24L01P_RX_FIFO_COUNT   3
 
#define _NRF24L01P_TX_FIFO_SIZE   32
#define _NRF24L01P_RX_FIFO_SIZE   32
 
#define _NRF24L01P_SPI_MAX_DATA_RATE     10000000
 
//0xCommand from spi 
#define _NRF24L01P_SPI_R_REGISTER            0x00
#define _NRF24L01P_SPI_W_REGISTER            0x20
#define _NRF24L01P_SPI_CMD_RD_RX_PAYLOAD     0x61   
#define _NRF24L01P_SPI_CMD_WR_TX_PAYLOAD     0xa0
#define _NRF24L01P_SPI_CMD_FLUSH_TX          0xe1
#define _NRF24L01P_SPI_CMD_FLUSH_RX          0xe2
#define _NRF24L01P_SPI_CMD_REUSE_TX_PL       0xe3
#define _NRF24L01P_SPI_CMD_R_RX_PL_WID       0x60
#define _NRF24L01P_SPI_CMD_W_ACK_PAYLOAD     0xa8
#define _NRF24L01P_SPI_CMD_W_TX_PYLD_NO_ACK  0xb0
#define _NRF24L01P_SPI_CMD_NOP               0xff
 
//Registers define address
#define _NRF24L01P_REG_CONFIG                0x00
#define _NRF24L01P_REG_EN_AA                 0x01
#define _NRF24L01P_REG_EN_RXADDR             0x02
#define _NRF24L01P_REG_SETUP_AW              0x03
#define _NRF24L01P_REG_SETUP_RETR            0x04
#define _NRF24L01P_REG_RF_CH                 0x05
#define _NRF24L01P_REG_RF_SETUP              0x06
#define _NRF24L01P_REG_STATUS                0x07
#define _NRF24L01P_REG_OBSERVE_TX            0x08
#define _NRF24L01P_REG_RPD                   0x09
#define _NRF24L01P_REG_RX_ADDR_P0            0x0a
#define _NRF24L01P_REG_RX_ADDR_P1            0x0b
#define _NRF24L01P_REG_RX_ADDR_P2            0x0c
#define _NRF24L01P_REG_RX_ADDR_P3            0x0d
#define _NRF24L01P_REG_RX_ADDR_P4            0x0e
#define _NRF24L01P_REG_RX_ADDR_P5            0x0f
#define _NRF24L01P_REG_TX_ADDR               0x10
#define _NRF24L01P_REG_RX_PW_P0              0x11
#define _NRF24L01P_REG_RX_PW_P1              0x12
#define _NRF24L01P_REG_RX_PW_P2              0x13
#define _NRF24L01P_REG_RX_PW_P3              0x14
#define _NRF24L01P_REG_RX_PW_P4              0x15
#define _NRF24L01P_REG_RX_PW_P5              0x16
#define _NRF24L01P_REG_FIFO_STATUS           0x17
#define _NRF24L01P_REG_DYNPD                 0x1c
#define _NRF24L01P_REG_FEATURE               0x1d
 
#define _NRF24L01P_REG_ADDRESS_MASK          0x1f
 
// CONFIG register:
#define _NRF24L01P_CONFIG_PRIM_RX        (1<<0) // 0000 0001 = 0x01
#define _NRF24L01P_CONFIG_PWR_UP         (1<<1) // 0000 0010 = 0x02
#define _NRF24L01P_CONFIG_CRC0           (1<<2) // 0000 0100 = 0x04
#define _NRF24L01P_CONFIG_EN_CRC         (1<<3) // 0000 1000 = 0x08
#define _NRF24L01P_CONFIG_MASK_MAX_RT    (1<<4) // 0001 0000 = 0x10
#define _NRF24L01P_CONFIG_MASK_TX_DS     (1<<5) // 0010 0000 = 0x20
#define _NRF24L01P_CONFIG_MASK_RX_DR     (1<<6) // 0100 0000 = 0x40

//Mask to delete single bits in CONFIG register 
#define _NRF24L01P_CONFIG_NO_PRIM_RX     0xfe   // 1111 1110
#define _NRF24L01P_CONFIG_NO_PWR_UP      0xfd   // 1111 1101
#define _NRF24L01P_CONFIG_NO_CRC0        0xfb   // ... 
#define _NRF24L01P_CONFIG_NO_EN_CRC      0xf7
#define _NRF24L01P_CONFIG_NO_MASK_MAX_RT 0xef
#define _NRF24L01P_CONFIG_NO_MASK_TX_DS  0xdf
#define _NRF24L01P_CONFIG_NO_MASK_RX_DR  0xbf
 
#define _NRF24L01P_CONFIG_CRC_MASK       (_NRF24L01P_CONFIG_EN_CRC|_NRF24L01P_CONFIG_CRC0)
#define _NRF24L01P_CONFIG_CRC_NONE       (0)
#define _NRF24L01P_CONFIG_CRC_8BIT       (_NRF24L01P_CONFIG_EN_CRC)
#define _NRF24L01P_CONFIG_CRC_16BIT      (_NRF24L01P_CONFIG_EN_CRC|_NRF24L01P_CONFIG_CRC0)
 
// EN_AA register:
#define _NRF24L01P_EN_AA_NONE            0
 
// EN_RXADDR register:
#define _NRF24L01P_EN_RXADDR_NONE        0
 
// SETUP_AW register:
#define _NRF24L01P_SETUP_AW_AW_MASK      (0x3<<0)
#define _NRF24L01P_SETUP_AW_AW_3BYTE     (0x1<<0)
#define _NRF24L01P_SETUP_AW_AW_4BYTE     (0x2<<0)
#define _NRF24L01P_SETUP_AW_AW_5BYTE     (0x3<<0)
 
// SETUP_RETR register:
#define _NRF24L01P_SETUP_RETR_NONE       0
 
// RF_SETUP register:
#define _NRF24L01P_RF_SETUP_RF_PWR_MASK          (0x3<<1)
#define _NRF24L01P_RF_SETUP_RF_PWR_0DBM          (0x3<<1)
#define _NRF24L01P_RF_SETUP_RF_PWR_MINUS_6DBM    (0x2<<1)
#define _NRF24L01P_RF_SETUP_RF_PWR_MINUS_12DBM   (0x1<<1)
#define _NRF24L01P_RF_SETUP_RF_PWR_MINUS_18DBM   (0x0<<1)
 
#define _NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT       (1 << 3)
#define _NRF24L01P_RF_SETUP_RF_DR_LOW_BIT        (1 << 5)
#define _NRF24L01P_RF_SETUP_RF_DR_MASK           (_NRF24L01P_RF_SETUP_RF_DR_LOW_BIT|_NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT)
#define _NRF24L01P_RF_SETUP_RF_DR_250KBPS        (_NRF24L01P_RF_SETUP_RF_DR_LOW_BIT)
#define _NRF24L01P_RF_SETUP_RF_DR_1MBPS          (0)
#define _NRF24L01P_RF_SETUP_RF_DR_2MBPS          (_NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT)
 
// STATUS register:
#define _NRF24L01P_STATUS_TX_FULL        (1<<0)
#define _NRF24L01P_STATUS_RX_P_NO        (0x7<<1)
#define _NRF24L01P_STATUS_MAX_RT         (1<<4)
#define _NRF24L01P_STATUS_TX_DS          (1<<5)
#define _NRF24L01P_STATUS_RX_DR          (1<<6)
 
// RX_PW_P0..RX_PW_P5 registers:
#define _NRF24L01P_RX_PW_Px_MASK         0x3F
 
#define _NRF24L01P_TIMING_Tundef2pd_us     100000   // 100mS
#define _NRF24L01P_TIMING_Tstby2a_us          130   // 130uS
#define _NRF24L01P_TIMING_Thce_us              10   //  10uS
#define _NRF24L01P_TIMING_Tpd2stby_us        4500   // 4.5mS worst case
#define _NRF24L01P_TIMING_Tpece2csn_us          4   //   4uS

typedef Gpio <GPIOB_BASE,15> mosi;
typedef Gpio <GPIOB_BASE,14> miso;
typedef Gpio <GPIOB_BASE,13> sck;
typedef Gpio <GPIOB_BASE,12> cs;
typedef Gpio <GPIOB_BASE,11> ce;

// thanks to this enum I can check the status of transceiver via software
// unless read the register.
typedef enum {
_NRF24L01P_MODE_UNDEFINED,
_NRF24L01P_MODE_POWER_DOWN,
_NRF24L01P_MODE_STANDBY,
_NRF24L01P_MODE_RX_MODE,
_NRF24L01P_MODE_TX_MODE,
} NRF24L01P_MODE;


typedef enum {
    _TX_FIFO_EMPTY,
    _TX_FIFO_NOT_EMPTY,
    _TX_FIFO_FULL // only when all slots are busy
} TX_FIFO_STATE;


int nrf24l01p_mode;
int tx_fifo_status;

NRF24L01P::NRF24L01P() {
    
   spi = new spi();
   mosi::mode(Mode::ALTERNATE);
   mosi::alternateFunction(5);
   miso::mode(Mode::ALTERNATE);
   miso::alternateFunction(5);
   sck::mode(Mode::ALTERNATE);
   sck::alternateFunction(5);
   cs::mode(Mode::OUTPUT);
   cs::high();
   
   nrf24l01p_mode = _NRF24L01P_MODE_UNDEFINED; // starts with an undefined state
   tx_fifo_status = _TX_FIFO_EMPTY; // the tx_fifo starts empty obviously
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
   unsigned char current_config , new_config;
   
   current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
   new_config = current_config | _NRF24L01P_CONFIG_PWR_UP; // put to 1 the PWR_UP bit in CONFIG register
   
   writeRegister(_NRF24L01P_REG_CONFIG, new_config );
   
   //now have to wait to worst time from power_down to standby ( charts for info )
   
   sleep(_NRF24L01P_TIMING_Tpd2stby_us);
   nrf24l01p_mode = _NRF24L01P_MODE_STANDBY; // now we are in standby mode 
}

void NRF24L01P::powerDown()
{
 
unsigned char current_config , new_config;
current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
new_config = current_config & _NRF24L01P_CONFIG_NO_PWR_UP; // put to 0 the PWR_UP bit in CONFIG register

writeRegister(_NRF24L01P_REG_CONFIG, new_config );
nrf24l01p_mode = _NRF24L01P_MODE_POWER_DOWN; 

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
 In order to call this function you have to call power_up, PWR_UP MUST be 1! 
 */
void NRF24L01P::setReceiveMode()
{
   unsigned char current_config , new_config;

   current_config = readRegister(_NRF24L01P_REG_CONFIG); // retreive the current status of CONFIG register
   
   if( nrf24l01p_mode != _NRF24L01P_MODE_STANDBY; ) // security control, if somebody call this function before settings transceiver
     {
       fprintf(stderr, "You have to call powerUp first\n");
       return;
     }
   
   new_config = current_config | _NRF24L01P_CONFIG_PRIM_RX ; // put to 1 the PRIM_RX bit in CONFIG register 
   
   writeRegister(_NRF24L01P_REG_CONFIG, new_config);
   ce::high();
   
   sleep(_NRF24L01P_TIMING_Tstby2a_us);
   
   nrf24l01p_mode = _NRF24L01P_MODE_RX_MODE;
}

/**
 * used to send data to TX_FIFO_BUFFER through the SPI
 * @param data: data to send
 */
void NRF24L01P::sendDataToTx(int data)
{
    cs::low();
    spi.send(_NRF24L01P_SPI_CMD_WR_TX_PAYLOAD ); // request to write on the TX_BUFFER a payload
    spi.send(data);
    cs::high();
    tx_fifo_status = _TX_FIFO_NOT_EMPTY;
}

/**
 * used to take data from RX_FIFO_BUFFER through SPI 
 * @return 
 */
int NRF24L01P::receiveDataFromRx()
{   
    int content;
    cs::low();
    spi.send(_NRF24L01P_SPI_CMD_RD_RX_PAYLOAD);
    content = spi.receive();
    cs::high();
}

/**
 * This function is used to read the RPD bit that 
 * tells if a valid RF signal >-64dbm is detected. 
 * This means that a NRF24L01p is near us, so we can send our payload.
 * @return 
 */
unsigned char NRF24L01P::readRPD()
{
   if( nrf24l01p_mode != _NRF24L01P_MODE_TX_MODE; ) // security check
     {
       fprintf(stderr, "Failed to read RPD, must be in TX_MODE\n");
       return;
     }
   
   return readRegister(_NRF24L01P_REG_RPD);
    
}

/**
 * Remember to call this function after call to returnStandBy-I
 * Before call this function a packet in TX must be present, so I check if TX is empty or not 
 */
void NRF24L01P::setTransmitMode()
{
  if( nrf24l01p_mode != _NRF24L01P_MODE_STANDBY; ) // security check
     {
       fprintf(stderr, "Failed to set transmission mode, call ce_down or check pwr_up bit\n");
       return;
     }   
  if( tx_fifo_status == _TX_FIFO_EMPTY ) // with this I avoid to finish in Standby-II ( possible operating mode , not suggested )
     {
       fprintf(stderr, "Failed to set transmission mode,there is no data to transmit, push something to TX_BUFFER\n");
       return;
     }
  
  unsigned char current_config , new_config;
  current_config = readRegister(_NRF24L01P_REG_CONFIG);
  new_config = current_config & _NRF24L01P_CONFIG_NO_PRIM_RX; // actually, this is 1111 1110 and preserve previous config, deleting the PRIM_RX
  
  writeRegister(_NRF24L01P_REG_CONFIG , new_config );
  ce::high() // for more than 10us 
  
  nrf24l01p_modemode = _NRF24L01P_MODE_TX_MODE;
  
  //here now starts the transmission packet flow charts, remember to change mode in the function
  //that handles the interrupt and to set the TX_FIFO to empty ( if empty naturally )
}

void NRF24L01P::writeRegister(unsigned char regAddress, unsigned char regData)
{
    if(nrf24l01p_modemode > _NRF24L01P_MODE_POWER_DOWN  )
       {
        fprintf(stderr, " WriteRegister only in power_down or stand_by mode"); //check manual for this 
        return;
       }
    unsigned char request;
    request = regAddress | _NRF24L01P_SPI_W_REGISTER;
   
    cs::low();
    
    spi.send(request);
    spi.send(regData); // this I think need a revert , Data has been to delivered LSB to MSByte
    
    cs::high();
    
}

/**
 * This function read a general register on the NRF24L01P
 * 
 * @param regAddress: the address of the register, see above for the define
 * @return : the value in hex of the register
 * 
 */
unsigned char NRF24L01P::readRegister(unsigned char regAddress) //arriva su 1 byte
{
    // unsigned char request; serve solo 1 byte
    // request = regAddress; tanto questo è in | bit a bit con 0x00 ( READ ) e in & bit a bit con una MASK 0x1f
    unsigned char value;
    
    //every spi commands must starts with a high to low cs signal
    cs::low();
    
    spi.send(regAddress);
    value = spi.receive();
    
    cs::high();
    
    return value;
}

/**
 * 
 * the status register is passe via miso after a dummy write on mosi
 * @return : the value in hex of status register of NRF24L01P
 */
unsigned char NRF24L01P::readStatusRegister()
{
    unsigned char value;
    cs::low();
    value=spi.receive(); // in this there is a dummy write of 0 on MOSI
    cs::high();
    return value; // need a reverse? it arrives form LSB to MSByte

}

void __attribute__((naked)) EXTI0_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z16EXTI0HandlerImplv");
    restoreContext();
}

void __attribute__((used)) EXTI0HandlerImpl()
{
    EXTI->PR=EXTI_PR_PR0; //viene resettato il registro che permette di uscire dalla chiamata a interrupt 
    
   
}
