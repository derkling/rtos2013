#ifndef SOCIAL_WIRELESS_H
#define SOCIAL_WIRELESS_H


/*!@brief Inizializza il modulo NRF24LR: configura SPI2, alloca la coda di trasmissione, crea il thread di trasmissione e il thread interrupt
 */
void init(void);

/*!@brief Trasmissione via radio della stringa passata come parametro
 * @param payload: stringa da trasmettere
 * @retval Ritorna -1 se c'è stato un errore, 0 altrimenti
 * @note La trasmissione non è istantanea. Il payload è inserito in una coda e trasmesso appena possibile.
 * @note La funzione si blocca fino a quando non ottiene il permesso di scrittura in coda
 */
int sendData(char* payload);

#endif /* SOCIAL_WIRELESS_H */
