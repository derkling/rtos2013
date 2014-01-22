#ifndef QUEUE_H
#define	QUEUE_H
#define QUEUE_LENGHT 7

//TYPEDEF

/*!@brief Elemento che definisce una coda.
 *        Se queue_t queue è la nostra variabile di tipo queue_t, allora:
 *              -queue.buffer: indica gli elementi della coda gestiti come array circolare;
 *              -queue.head: è il puntatore alla testa della coda;
 *              -queue.next: è il puntatore al primo spazio dell'array in memoria libero;
 *              -queue.freeSpace: indica quanto spazio è ancora disponibile nell'array della coda.
 *        Per inizializzare correttamete la variabile è necessario utilizzare la funzione
 *        queueInizializer().
 */
typedef struct{
    char buffer[QUEUE_LENGHT];
    char* head;
    char* next;   
    int freeSpace; 
}queue_t;

//FUNZIONI

/*!@brief Crea un nuovo dato tipo queue_t correttamente inizializzato.
 * @retval Ritorna l'elemento queue_t correttamente inizializzato.
 */
void queueInizializer(queue_t* queue);

/*!@brief Aggiunge un dato alla coda.
 * @param data: stringa contenente il dato da inserire in coda.
 * @param queue: coda da gestire.
 * @retval Ritorna -1 se la coda è piena, 0 altrimenti.
 */
int addData(char* data, queue_t* queue);

/*!@brief Serve la coda. Si consiglia di utilizzare prima la funzione 
 *        queueIsEmpty() per controllare se la coda ha elementi.
 * @param queue: coda da gestire.
 * @param data: stringa in cui si vuole copiare il dato preso dalla coda.
 * @retval Ritorna il valore del primo dato da servire in coda, se non ci sono 
 *         elementi in coda ritorna NULL.
 */
char* enqueue(queue_t* queue, char* data);

/*!@brief Controlla se la coda è vuota.
 * @param queue: coda da gestire.
 * @retval Ritorna true se la coda è vuota, false altrimenti.
 */
bool queueIsEmpty(queue_t* queue);
#endif	/* QUEUE_H */
