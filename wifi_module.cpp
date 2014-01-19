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
typedef Gpio<GPIOD_BASE,14> redLed;

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
    redLed::high();
    printf("Sono nell'interrupt");
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
    IRQ::mode(Mode::INPUT_PULL_UP);
    SYSCFG->EXTICR[1] = SYSCFG_EXTICR1_EXTI1_PA;
    EXTI->IMR |= EXTI_IMR_MR1; /*Periferica che gestisce gli external interrupt, è per il gpio 1*/
    EXTI->RTSR &= ~EXTI_RTSR_TR1; /*Vado a verificare durante il fronte di salita*/
    EXTI->FTSR |= EXTI_FTSR_TR1;
    NVIC_EnableIRQ(EXTI1_IRQn); /*Abilitano il controller dell'interrupt, passando il nome e poi la priorità*/
    NVIC_SetPriority(EXTI1_IRQn,15); //Low priority
    
}
void *wifi_start(void *arg)
{
    char data[32] = "prova";
    nRF24L01P *wifi;
    wifi = new nRF24L01P();
    greenLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    configureModuleInterrupt();
    wifi->test_transmit();
    greenLed::high();
    usleep(1000000);
    greenLed::low();
    for(;;){
        greenLed::high();
        wifi->transmit(6,data);
        printf("Ho trasmesso\n");
        greenLed::low();
        usleep(2000000);
        /*wifi->receive(0,data,1);
        printf("Received data: %d",*data);
        greenLed::high();
        usleep(1000000);
        greenLed::low();
        num_step = wifi->receive();
        if (trasmission){
            wifi->transmit(num_step);
        }
        else{
            wifi->receive();
        }
    */
    }
    
   
    printf("Hello world, write your application here\n");
    
}
