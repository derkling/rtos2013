#include <cstdio>
#include "miosix.h"
#include "spi_driver.h"
#include "nRF24L01P.h"
#include "arch/cortexM4_stm32f4/common/CMSIS/stm32f4xx.h"
#include <miosix/kernel/scheduler/scheduler.h>

using namespace std;
using namespace miosix;

bool trasmission;
int num_step=0;
static Thread *waiting=0;
char *data; //data received from air

typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOA_BASE,1> IRQ;


void invia(int num_passi){
    trasmission=true;
}


void __attribute__((naked)) EXTI1_IRQHandler(){
    saveContext();
    asm volatile("bl _Z16EXTI1HandlerImplv");
    restoreContext();
}

void __attribute__((used))EXTI1HandlerImpl(){
    EXTI->PR=EXTI_PR_PR1;
    if(waiting==0) return;
    waiting->IRQwakeup(); 
    if(waiting->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
        Scheduler::IRQfindNextThread();
    waiting=0;
}

void waitForModule(){
    FastInterruptDisableLock dLock; 
    waiting=Thread::IRQgetCurrentThread();
    while(waiting)
    {
        Thread::IRQwait(); 
        FastInterruptEnableLock eLock(dLock); 
        Thread::yield(); 
    }
}

void configureModuleInterrupt()
{
    IRQ::mode(Mode::INPUT);
    EXTI->IMR |= EXTI_IMR_MR1; /*Periferica che gestisce gli external interrupt, è per il gpio 0*/
    EXTI->RTSR |= EXTI_RTSR_TR1; /*Vado a verificare durante il fronte di salita*/
    NVIC_EnableIRQ(EXTI1_IRQn); /*Abilitano il controller dell'interrupt, passando il nome e poi la priorità*/
    NVIC_SetPriority(EXTI1_IRQn,15); //Low priority
    
}
void *wifi_start(void *arg)
{
    char data[32] = "prova";
    nRF24L01P *wifi;
    wifi = new nRF24L01P();
    greenLed::mode(Mode::OUTPUT);
    configureModuleInterrupt();
    wifi->test();
    greenLed::high();
    usleep(1000000);
    greenLed::low();
    for(;;){
        printf("wait module");
        waitForModule();
        printf("ho ricevuto qlcs\n");
        //num_step = wifi->receive();
        /*if (trasmission){
            wifi->transmit(num_step);
        }
        else{
            wifi->receive();
        }
    */
    }
   
    printf("Hello world, write your application here\n");
    
}
