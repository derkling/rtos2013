#ifndef SPI_H
#define SPI_H

#include"nRF24L01.h"

//enable the spi and various gpio for comunicating with the wireless module
void configureSpi(void);

//alza il pin di alimentazione del modulo wireless
void powerLineUp(void);

//abbassa il pin di alimentazione del modulo wireless
void powerLineDown(void);

//alza il bit CE
void chipEnable(void);

//abbassa il bit CE
void chipDisable(void);

//manda via spi un comando e eventuali dati
int spiSendCommandWriteData(uint8_t,uint8_t,uint8_t*,uint8_t*,int);

//manda via spi un comando e riceve dati
int spiSendCommandReadData(uint8_t,uint8_t,uint8_t*,uint8_t*,int);

#endif //SPI_H
