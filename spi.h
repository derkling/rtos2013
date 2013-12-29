#ifndef SPI_H
#define SPI_H

#include"nRF24L01.h"

//enable the spi and various gpio for comunicating with the wireless module
void configureSpi(void);

//alza il bit CE
void chipEnable(void);

//abbassa il bit CE
void chipDisable(void);

//manda via spi il comando command e l'indirizzo addr(se il comando non prevede indirizzo mandare COMMAND_WITHOUT_ADDRESS
int spiSendCommand(uint8_t,uint8_t,uint8_t*);

#endif //SPI_H
