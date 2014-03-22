
#include <cstdio>
#include "miosix.h"
#include <miosix/kernel/scheduler/scheduler.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Wifi-Module/NRF24L01P.h"
#include "Podometer/pedometer.h"
#include "Speaker/slice-and-play.h"


using namespace std;
using namespace miosix;

typedef Gpio <GPIOA_BASE,0> button;

//Exclusive access to out_data is indirect provided by the mutex modality 
static int out_data = 0; //this is a global variable set by podometer thread
static int in_data = -1; //this is the data readed from other devices by the module 

static Thread *waiting=0; //waiting for module's irq
static Thread *waiting1=0; //waiting for button's irq 

static pthread_t irq_thread;
static pthread_t send_thread;
static pthread_t pedometerThread;
static pthread_t buttonThread;

static pthread_mutex_t modality=PTHREAD_MUTEX_INITIALIZER; //Mutex that prevent misconfiguration trasm/receive

NRF24L01P* module; 
Pedometer* pedometer;


void *startPedometer(void *arg) {
    miosix::Thread::getCurrentThread()->setPriority(2); //High priority to the podometer's counter 
    pedometer->start(); //Let's start counting 
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
     
     if(out_data < in_data) //If my steps are lower than the other
       {
        ring::instance().looser_Song(100);  //sound library!   
       }
     else
       {
        ring::instance().victory_Song(100); 
       }
     
     module->resetRXirq(); //reset rx_dr irq 
     printf("Received %d\n", in_data); //sugar      
     module->flushRx(); //flush the rx to prevent full buffer 
     pthread_mutex_unlock(&modality); //release the mutex
     continue;
    }
}

/**
 * Thread's function that send in the air the actual number
 * of steps done every 1 sec
 * @param arg
 * @return 
 */
void *send_handler(void* arg)
{
    char * pointer = (char*)&out_data; // char pointer to global int variable ( scan byte per byte the integer)
    while(true)
    {
        usleep(1000000); //every 1 second transmit 
        pthread_mutex_lock(&modality); //lock the modality 
        
        out_data = pedometer->getSteps(); //Update the out_data with current steps 
        module->TrasmitData(pointer,4); //Pointer now points to the updated value 
        printf("Transmitted %d\n", out_data); //Sugar 
        
        pthread_mutex_unlock(&modality); //release modality 
    }
}

void *button_handler(void*arg)
{
    //configuration of button's irq
    
    button::mode(Mode::INPUT_PULL_DOWN);
    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR |= EXTI_RTSR_TR0;
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI0_IRQn,15); //Low priority
    
    for(;;)
    {
    FastInterruptDisableLock dLock;
    waiting1=Thread::IRQgetCurrentThread();
    while(waiting1)
    {
        Thread::IRQwait();
        FastInterruptEnableLock eLock(dLock);
        Thread::yield();
    }
    
    ring::instance().play_n_of_step(out_data,100);  
   }


}


int main(){
    
    //Initialize and starts the podometer
    pedometer = pedometer->get_instance();
   
    //Initialize and configure the NRF24L01P for this purpose
    module = new NRF24L01P();
    module->powerUp(); //power up the module
    module->configureInterrupt(); //configure the irq on PA1
    module->disableAllAutoAck(); //disabling all auto-ack on all the pipe
    module->setStaticPayloadSize(4); // static payload size 4 byte ( due to the fact that we'll transmit only integer numbers)
    module->setReceiveMode(); //put module in receive mode 
    module->maskIrq(2); //Masking the tx_ds and max_rt irq, actually we don't need them 

    //Thread creation 
    pthread_create(&pedometerThread,NULL,&startPedometer, NULL); //launch podometer's thread 
    pthread_create(&irq_thread,NULL,&irq_handler,NULL); //thread that handle the receiving of a message from other boards 
    pthread_create(&send_thread,NULL,&send_handler,NULL); //thread that handle the transmission of our steps every 500ms 
    pthread_create(&buttonThread,NULL,&button_handler,NULL);
    //wait the guys! 
    pthread_join(irq_thread,NULL);
    pthread_join(send_thread,NULL);
    pthread_join(pedometerThread,NULL);
    pthread_join(buttonThread,NULL);

}

/**
 * Handling of irq , this one is the predefined function that 
 * respond to an irq on EXTI1 in the vector table 
 */
void __attribute__((naked)) EXTI1_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z16EXTI1HandlerImplv"); //Jump to C++ function from asm
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


//definitin of button irq handler in the vector table 
void __attribute__((naked)) EXTI0_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z16EXTI0HandlerImplv"); //jump to c++ function
    restoreContext();
}

void __attribute__((used)) EXTI0HandlerImpl()
{
    EXTI->PR=EXTI_PR_PR0; //viene resettato il registro che permette di uscire dalla chiamata a interrupt 
    
    if(waiting1==0) return;
    waiting1->IRQwakeup(); // risveglia il thread 
    if(waiting1->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
		Scheduler::IRQfindNextThread();
    waiting1=0;
}