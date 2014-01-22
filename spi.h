#ifndef SPI_H
#define SPI_H

#include"nRF24L01.h"

/*! @brief Configura la periferica spi2 e i gpio usati per la comunicazione con il modulo wireless
 */
void configureSpi(void);

/*! @brief Alza il pin per l'alimentazione del modulo wireless
 */
void powerLineUp(void);

/*! @brief Abbassa il pin di alimentazione del modulo wireless
 */
void powerLineDown(void);

/*! @brief Alza il bit di enable del modulo wireless
 */
void chipEnable(void);

/*! @brief Abbassa il bit di CE 
 */
void chipDisable(void);

/*! @brief Manda via spi comando e dati al modulo wireless. Primitiva bloccante, non torna finchè errore o invio completo
 *  @param command: comando da inviare
 *  @param addr: indirizzo del registro del modulo wireless(se comando senza indirizzo usare COMMAND_WITHOUT_ADDRESS)
 *  @param sr: puntatore a uint8_t dove verrà scritto lo status register ricevuto dal modulo wireless
 *  @param data: puntatore ai dati da inviare(se ci sono) dopo il comando. Verranno inviati da data[0] a data[lenght-1].
 *  	   ATTENZIONE i dati sono LSByte
 *  @param lenght: numero di byte(di dati) da inviare dopo il comando
 *  @retval -1 se errore, 1 se inviato
 */
int spiSendCommandWriteData(uint8_t command, uint8_t addr, uint8_t* sr, uint8_t* data, int lenght);

/*! @brief manda via spi comando e legge dati forniti dal modulo wireless. Primitiva bloccante, non torna finchè errore o invio completo
 *  @param command: comando da inviare
 *  @param addr: indirizzo del registro nel modulo wireless(se comando senza indirizzo usare COMMAND_WITHOUT_ADDRESS)
 *  @param sr: puntatore a uint8_t dove verrà scritto lo status register ricevuto dal modulo wireless
 *  @param data: puntatore ai dati ricevuti dopo il comando. Verranno riempiti da data[0] a data[lenght-1] in ordine di ricezione.
 *  	   ATTENZIONE la funzione non controllerà se la zona di memoria non è stata allocata correttamente per contenere 
 *  	   tutti i dati attesi e si ricorda che come per la primitiva spiSendCommandWriteData i dati sono LSByte
 *  @param lenght: numero di byte(di dati) da ricevere dopo aver inviato il comando
 *  @retval -1 se errore, 1 se inviato
 */
int spiSendCommandReadData(uint8_t command, uint8_t addr, uint8_t* sr, uint8_t* data, int lenght);

#endif //SPI_H
