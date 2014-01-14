#include "queue.h"
#define SEPARATORE '\0'

//FUNZIONI
int addData (char* data, queue_t queue){
    if (queue.freeSpace != 0 || (strlen(data) + 1) > queue.freeSpace){
        //coda piena
        return -1;
    }
    
    for (int i = 0; data[i] != '\0'; i++){
        *(queue.next) = data[i];
        queue.freeSpace--;
        //controllare la correttezza
        if (queue.next + 1 == &(queue.buffer[QUEUE_LENGHT])){
            queue.next = queue.buffer;
        } else {
            queue.next++;
        }
    }
    
    *(queue.next) = SEPARATORE;
    queue.freeSpace--;
    queue.next++;
    
    return 0;
}

char* enqueue(queue_t queue){
    int dataSize = 1;
    char* pnt = queue.head;
    while(*(pnt) != SEPARATORE){
        dataSize++;
        if (pnt == &(queue.buffer[QUEUE_LENGHT])){
            pnt = queue.buffer;
        } else {
            pnt++;
        }
    }
    char data[dataSize];
    for (int i = 0; *(queue.head) != SEPARATORE; i++){
        data[i] = *(queue.head);
        queue.freeSpace++;
        if (queue.head == &(queue.buffer[QUEUE_LENGHT])){
            queue.head = queue.buffer;
        } else {
            queue.head++;
        }
    }
    queue.head++;
    queue.freeSpace++;
    data[dataSize - 1] = '\0';
    
    return data;
}

bool isEmpty(queue_t queue){
    if (queue.freeSpace != 0){
        return false;
    } else {
        return true;
    }
}