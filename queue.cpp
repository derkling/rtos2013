#include "queue.h"
#include <string.h>

void queueInizializer(queue_t* queue){
    //inizializzo l'head
    queue->head = queue->buffer;
    //inizializzo il next
    queue->next = queue->head;
    //inizializzo la quantita' di spazio libero
    queue->freeSpace = QUEUE_LENGHT;
    //ritorno queue inizializzato
}

int queuePush (char* data, queue_t* queue){
    
    int size = strlen(data);
    
    //controllo della dimensione del nuovo dato da inserire
    if ((queue->freeSpace == 0) || (size + 1 > queue->freeSpace) || (size <= MAX_LENGHT_PAYLOAD)){
        return -1;
    }
    
    //aggiunta del dato nella coda
    for (int i = 0; i < size; i++){
        *(queue->next) = data[i];
        queue->freeSpace--;
        if (queue->next + 1 == &(queue->buffer[QUEUE_LENGHT])){
            queue->next = queue->buffer;
        } else {
            queue->next++;
        }
    }
    
    *(queue->next) = SEPARATORE;
    queue->freeSpace--;
    queue->next++;
    
    return 0;
}

void queuePop(queue_t* queue, char* dest){
    int dataSize = 1;
    char* pnt = queue->head;
    //calcolo dimensione del dato da spilare
    while(*(pnt) != SEPARATORE){
        dataSize++;
        if (pnt + 1 == &(queue->buffer[QUEUE_LENGHT])){
            pnt = queue->buffer;
        } else {
            pnt++;
        }
    }
    
   //copio il dato nella stringa di destinazione 
   for (int i = 0; *(queue->head) != SEPARATORE; i++){
        dest[i] = *(queue->head);
        queue->freeSpace++;
        if (queue->head + 1 == &(queue->buffer[QUEUE_LENGHT])){
            queue->head = queue->buffer;
        } else {
            queue->head++;
        }
    }
    queue->head++;
    queue->freeSpace++;
    dest[dataSize - 1] = '\0';
}

bool queueIsEmpty(queue_t* queue){
    if (queue->freeSpace == QUEUE_LENGHT){
        return true;
    } else {
        return false;
    }
}
