
#include <cstdio>
#include "miosix.h"
#include <miosix/kernel/scheduler/scheduler.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "NRF24L01P.h"
#include "pedometer.h"
#include "slice-and-play.h"

using namespace std;
using namespace miosix;

static int out_data = 0; //this is a global variable set by podometer thread (podometer must initialize this variable )
static int in_data = -1; //this is the data readed from other devices by the 

static Thread *waiting=0;

static pthread_t irq_thread;
static pthread_t send_thread;
static pthread_t pedometerThread;

static pthread_mutex_t modality=PTHREAD_MUTEX_INITIALIZER; //Mutex that prevent misconfiguration trasm/receive

NRF24L01P* module;
Pedometer* pedometer;

void *startPedometer(void *arg) {
    miosix::Thread::getCurrentThread()->setPriority(2);
    pedometer->start();
}

void *irq_handler(void* arg)
{    
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
     //Out of there I received an rx_dr irq for sure, because the other are masked 
     
     pthread_mutex_lock(&modality); //Can I put in receive mode? 
   
     in_data = module->receiveDataFromRx(); //Retreive the data received from others 
     out_data = pedometer->getSteps(); //Retreive the current steps from pedometer 
     
     if(out_data < in_data)
       {
        ring::instance().looser_Song(100);       
       }
     else
       {
        ring::instance().victory_Song(100); 
       }
     
     module->resetRXirq(); //reset rx_dr irq 
     module->notifyRX(); //blink led that notify receiving 
     printf("Received %d\n", in_data);     
     module->flushRx(); //flush the rx to prevent full buffer 
     pthread_mutex_unlock(&modality); //release the mutex
     continue;
    }
}

void *send_handler(void* arg)
{
    char * pointer = (char*)&out_data;
    while(true)
    {
        usleep(500000); //every 500ms transmit (old 5000000)
        pthread_mutex_lock(&modality);
        out_data = pedometer->getSteps(); //Update the out_data with current steps
        module->TrasmitData(pointer,4); //Pointer now points to the updated value 
        printf("Transmitted %d\n", out_data);
        
        pthread_mutex_unlock(&modality);
    }
}

int main(){
    
    //Initialize and starts the podometer
    pedometer = pedometer->get_instance();
   
    module = new NRF24L01P();
    module->powerUp(); //power up the module
    module->configureInterrupt();
    module->disableAllAutoAck(); //disabling all auto-ack on all the pipe
    module->setStaticPayloadSize(4); // static payload size 4 byte ( due to the fact that we'll transmit only integer numbers)
    module->setReceiveMode();
    module->maskIrq(2);

    pthread_create(&pedometerThread,NULL,&startPedometer, NULL); //launch podometer's thread 
    pthread_create(&irq_thread,NULL,&irq_handler,NULL);
    pthread_create(&send_thread,NULL,&send_handler,NULL);
    
    pthread_join(irq_thread,NULL);
    pthread_join(send_thread,NULL);
    pthread_join(pedometerThread,NULL);

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


