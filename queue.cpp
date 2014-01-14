#include <queue.h>
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
        if (queue.next + 1 == queue.buffer[QUEUE_LENGHT]){
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
    char data[strlen(queue.head)];
    strcpy(data, queue.head);
    queue.head = queue.head + strlen(queue.head) + 1;
    return data;
}

bool isEmpty(queue_t queue){
    if (queue.freeSpace != 0){
        return false;
    } else {
        return true;
    }
}