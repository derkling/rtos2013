
#include <cstdio>
#include "miosix.h"
#include <pthread.h>
#include <unistd.h>
#include <miosix/kernel/scheduler/scheduler.h>
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

static int out_data = 0; //this is a global variable set by podometer thread (podometer must initialize this variable )
static int in_data = -1; //global variable readed by sound thread and setted by our module 

static Thread *waiting=0;

static pthread_t irq_thread;
static pthread_t send_thread;

static pthread_mutex_t modality=PTHREAD_MUTEX_INITIALIZER; //Mutex that prevent misconfiguration trasm/receive
static pthread_mutex_t data=PTHREAD_MUTEX_INITIALIZER; //Mutex that prevent race condition on global data 

NRF24L01P* module;

void *irq_handler(void* arg)
{    printf("inside irq handler");

    while(true)
    {
     FastInterruptDisableLock dLock;
     waiting=Thread::IRQgetCurrentThread();
     while(waiting) //waiting for an interrupt (rx_dr)
          {
           Thread::IRQwait();
           FastInterruptEnableLock eLock(dLock);
           Thread::yield();
          }
     //Out of there I received an irq for sure 
     
     pthread_mutex_lock(&modality); //Can I put in receive mode? 
     if( (module->readStatusRegister() & 0x40 ) != 0 ) // I received an rx_dr irq 
       {
         in_data = module->receiveDataFromRx();
         //Chiamata al suono per il confronto [TODO]
         module->resetRXirq(); 
         module->notifyRX();
         printf("Received %d", in_data);
       }
     else
       {
         module->resetTXirq();
         printf("transmitted!!");
       }
     
     pthread_mutex_unlock(&modality);
    //remember reset IRQ 
    }

}

void *send_handler(void* arg)
{
    char * pointer = (char*)&out_data;
    printf("inside send threaed");
    while(true)
    {
        usleep(50000); //every 50000ms transmit 
        pthread_mutex_lock(&modality);
        module->TrasmitData(pointer,4);
        printf("Transmitted %d", out_data);
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
    
    pthread_create(&irq_thread,NULL,&irq_handler,NULL);
    pthread_create(&send_thread,NULL,&send_handler,NULL);
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


