
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

extern int out_data = 0; //this is a global variable set by podometer thread (podometer must initialize this variable )

extern int in_data = -1; //global variable readed by sound thread and setted by our module

static Thread *waiting=0;//global variable readed by interrupt handler

//ATTENTION: WITH INTEGRATION WE MUST HANDLE THE MUTUAL EXCLUSION TO ACCESS AT THIS GLOBAL VARS


 //setto le interrupt(stessa sia pe ricezione che fine trasmissione)
void __attribute__((naked)) EXTI1_IRQHandler(){

    saveContext();
    asm volatile("bl _Z16EXTI1HandlerImplv");
    restoreContext();

}


void __attribute__((used)) EXTI1HandlerImpl(){

    EXTI->PR = EXTI_PR_PR1;

    if(waiting==0) return;
    waiting->IRQwakeup();
    if(waiting->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority()) Scheduler::IRQfindNextThread();
    waiting=0;

    greenLed::high();
}


//thread che si occupa della trasmissione
//infinito, polling ogni 2 secondi su out_data
void *transThread(void *arg){

    for(;;){

        usleep(20000);
        //se il podometro mi ha settato la variabile (diversa da 0) vuol dire che devo trasmettere
        if(out_data != 0){

            char * pointer = (char*)&out_data;
             module->TrasmitData(pointer,4);
             out_data = 0;

        }


    }

}



   int init(){
    NRF24L01P* module = new NRF24L01P();


    module->powerUp();
    module->configureInterrupt();
    module->disableAllAutoAck();
    module->setStaticPayloadSize(4); // static payload size 4 byte ( due to the fact that we'll transmit only integer numbers)
    module->setReceiveMode();

    }



