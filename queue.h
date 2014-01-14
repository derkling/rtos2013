#ifndef QUEUE_H
#define	QUEUE_H
#define QUEUE_LENGHT 1024

//TYPEDEF
typedef struct{
    char buffer[QUEUE_LENGHT]; //elementi della coda
    char* head = buffer; //testa della coda.
    char* next = head; //prossimo elemento libero della coda
    bool isFull = false; //indica se la coda ha elementi
}queue;

//FUNZIONI
/*!@brief Aggiunge un dato alla coda.
 * @param data: stringa contenente il dato da inserire in coda.
 * @param queue: coda da gestire.
 * @retval Ritorna -1 se la coda è piena.
 */
int addData(char* data, queue q);

/*!@brief Serve la coda. Si consiglia di utilizzare prima la funzione isEmpty() 
 *        per controllare se la coda ha elementi.
 * @param queue: coda da gestire.
 * @retval Ritorna il valore del primo dato da servire in coda, se non ci sono 
 *         elementi in coda ritorna NULL.
 */
char* enqueue(queue q);

#endif	/* QUEUE_H */

