#ifndef QUEUE_H
#define	QUEUE_H

/*!@brief Aggiunge un dato alla coda.
 * @param data: stringa contenente il dato da inserire in coda.
 * @param queue: coda da gestire.
 * @retval Ritorna -1 se la coda è piena.
 */
int addData(char* data, char* queue);

/*!@brief Serve la coda. Si consiglia di utilizzare prima la funzione isEmpty() 
 *        per controllare se la coda ha elementi.
 * @param queue: coda da gestire.
 * @retval Ritorna il valore del primo dato da servire in coda, se non ci sono 
 *         elementi in coda ritorna NULL.
 */
char* enqueue(char* queue);

/*!@brief Controlla se ci sono elementi in coda.
 * @param queue: coda da gestire.
 * @retval Se ci sono elementi in coda ritorna false, altrimenti true.
 */
bool isEmpty(char* queue);

#endif	/* QUEUE_H */

