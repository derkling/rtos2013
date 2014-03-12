
#include <cstdio>
#include "miosix.h"
#include <miosix/kernel/scheduler/scheduler.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

static int out_data = 0; //this is a global variable set by podometer thread (podometer must initialize this variable )
static int in_data = -1; //global variable readed by sound thread and setted by our module 

static Thread *waiting=0;

static pthread_t irq_thread;
static pthread_t send_thread;

static pthread_mutex_t modality=PTHREAD_MUTEX_INITIALIZER; //Mutex that prevent misconfiguration trasm/receive

NRF24L01P* module;

void *irq_handler(void* arg)
{    
    printf("lanciato irq handler \n");
    for(;;)
    {
     FastInterruptDisableLock dLock;
     waiting=Thread::IRQgetCurrentThread();
     while(waiting) //waiting for an interrupt (rx_dr)
          {
           Thread::IRQwait();
           FastInterruptEnableLock eLock(dLock);
           Thread::yield();
          }
     printf("arrivato irq rx_dr\n");
     //Out of there I received an irq for sure 
     pthread_mutex_lock(&modality); //Can I put in receive mode? 
   
         in_data = module->receiveDataFromRx();
         //Chiamata al suono per il confronto [TODO]
         module->resetRXirq(); 
         module->notifyRX();
         printf("Received %d\n", in_data);
         module->flushRx();
     
     pthread_mutex_unlock(&modality);
     continue;
    //remember reset IRQ 
    }

}

void *send_handler(void* arg)
{
    char * pointer = (char*)&out_data;
    printf("inside send thread\n");
    while(true)
    {
        usleep(500000); //every 500ms transmit (old 5000000)
        pthread_mutex_lock(&modality);
        
        module->TrasmitData(pointer,4);
        printf("Transmitted %d\n", out_data);
        
        pthread_mutex_unlock(&modality);
    }
}

int main(){
    module = new NRF24L01P();
    module->powerUp(); //power up the module
    module->configureInterrupt();
    module->disableAllAutoAck(); //disabling all auto-ack on all the pipe
    module->setStaticPayloadSize(4); // static payload size 4 byte ( due to the fact that we'll transmit only integer numbers)
    module->setReceiveMode();
    module->maskIrq(2);

    pthread_create(&irq_thread,NULL,&irq_handler,NULL);
    pthread_create(&send_thread,NULL,&send_handler,NULL);
    
    pthread_join(irq_thread,NULL);
    pthread_join(send_thread,NULL);
}


void __attribute__((naked)) EXTI1_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z16EXTI1HandlerImplv");
    restoreContext();
}

void __attribute__((used)) EXTI1HandlerImpl()
{
    EXTI->PR=EXTI_PR_PR1; //Reset the register that permit to exit from IRQ call
    if(waiting==0) return;
    waiting->IRQwakeup();
    if(waiting->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority()) Scheduler::IRQfindNextThread();
    waiting=0;
}


