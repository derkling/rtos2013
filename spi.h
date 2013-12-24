#ifndef SPI_H
#define SPI_H

/* DEFINIZIONE COMANDI */
#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xa0
#define FLUSH_TX 0xd1
#define FLUSH_RX 0xd2
#define REUSE_TX_PL 0xd3
#define ACTIVATE 0x50
#define NOP 0xff
//i successivi sono attivabili solo dopo aver inviato il comando ACTIVATE e il dato ACTIVATE_BYTE
#define R_RX_PL_WID 0x60
#define W_ACK_PAYLOAD 0xa8
#define W_TX_PAYLOAD_NO_ACK 0xb0

/* DEFINIZIONE DATI PARTICOLARI */
#define ACTIVATE_BYTE 0x73

/* 
 * enable the spi and various gpio for comunicating with the wireless module
 */
void configureSpi(void);

//alza il bit CE
void chipEnable(void);

//abbassa il bit CE
void chipDisable(void);

#endif //SPI_H
