#include "queue.h"
#include <string.h>
#define SEPARATORE '\0'

//FUNZIONI

void queueInizializer(queue_t* queue){
    //inizializzo l'head
    queue->head = queue->buffer;
    //inizializzo il next
    queue->next = queue->head;
    //inizializzo la quantita' di spazio libero
    queue->freeSpace = QUEUE_LENGHT;
    //ritorno queue inizializzato
}

int addData (char* data, queue_t* queue){
    
    int size = strlen(data);
    if ((queue->freeSpace == 0) || (size + 1 > queue->freeSpace)){
        //coda piena
        return -1;
    }
    
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

char* enqueue(queue_t* queue){
    int dataSize = 1;
    char* pnt = queue->head;
    while(*(pnt) != SEPARATORE){
        dataSize++;
        if (pnt +1 == &(queue->buffer[QUEUE_LENGHT])){
            pnt = queue->buffer;
        } else {
            pnt++;
        }
    }
    char data[dataSize];
    for (int i = 0; *(queue->head) != SEPARATORE; i++){
        data[i] = *(queue->head);
        queue->freeSpace++;
        if (queue->head +1 == &(queue->buffer[QUEUE_LENGHT])){
            queue->head = queue->buffer;
        } else {
            queue->head++;
        }
    }
    queue->head++;
    queue->freeSpace++;
    data[dataSize - 1] = '\0';
    
    return data;
}

bool queueIsEmpty(queue_t* queue){
    if (queue->freeSpace == QUEUE_LENGHT){
        return true;
    } else {
        return false;
    }
}