#ifndef QUEUE_H
#define	QUEUE_H
#define QUEUE_LENGHT 1024

//TYPEDEF
typedef struct{
    char buffer[QUEUE_LENGHT]; //elementi della coda
    char* head = buffer; //testa della coda.
    char* next = head; //prossimo elemento libero della coda
     //indica se la coda ha elementi
    int freeSpace = QUEUE_LENGHT;
}queue_t;

//FUNZIONI
/*!@brief Aggiunge un dato alla coda.
 * @param data: stringa contenente il dato da inserire in coda.
 * @param queue: coda da gestire.
 * @retval Ritorna -1 se la coda è piena, 0 altrimenti.
 */
int addData(char* data, queue_t queue);

/*!@brief Serve la coda. Si consiglia di utilizzare prima la funzione isEmpty() 
 *        per controllare se la coda ha elementi.
 * @param queue: coda da gestire.
 * @retval Ritorna il valore del primo dato da servire in coda, se non ci sono 
 *         elementi in coda ritorna NULL.
 */
char* enqueue(queue_t queue);

/*!@brief Controlla se la coda è vuota.
 * @param queue: coda da gestire.
 * @retval Ritorna true se la coda è vuota, false altrimenti.
 */
bool isEmpty(queue_t queue);
#endif	/* QUEUE_H */
