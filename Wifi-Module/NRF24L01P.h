/* 
 * File:   NRF24L01P.h
 * Author: degrigis
 *
 * Created on January 9, 2014, 7:38 PM
 */

#ifndef NRF24L01P_H
#define	NRF24L01P_H
#include "spi.h"

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

// EN_AA register:
#define _NRF24L01P_EN_AA_NONE            0
 
// EN_RXADDR register:
#define _NRF24L01P_EN_RXADDR_NONE        0
 
// SETUP_AW register:
#define _NRF24L01P_SETUP_AW_AW_MASK      (0x3<<0)
#define _NRF24L01P_SETUP_AW_AW_3BYTE     (0x1<<0)
#define _NRF24L01P_SETUP_AW_AW_4BYTE     (0x2<<0)
#define _NRF24L01P_SETUP_AW_AW_5BYTE     (0x3<<0)
 
 
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

class NRF24L01P {
public:
    NRF24L01P();
    NRF24L01P(const NRF24L01P& orig);
    virtual ~NRF24L01P();
    void writeRegister(int regAddress, int regData);
    int readRegister(int regAddress);
    int readStatusRegister();
    void powerUp();
    void powerDown();
    void setReceiveMode();
    void setTransmitMode();
    void setStaticPayloadSize(int size);
    void disableAllAutoAck();
    void maskIrq(int w);
    void returnStandByI();
    int receiveDataFromRx();
    int readRPD();
    void configureInterrupt();
    void configureTxAddress(int n_byte);
    void flushTx();
    void flushRx();
    void TrasmitData(char* data_pointer , int dim);
    void resetModule();
    void showInternal();
    void setRfChannel(int channel);
    void setAirDataRate(int air_rate);
    void resetRXirq();
    void resetTXirq();
private:
    spi *spiDriver;
};

#endif	/* NRF24L01P_H */

