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
char bufferTransmit[96];
int counter = 0;


pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t buff=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t spi=PTHREAD_MUTEX_INITIALIZER;

typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOA_BASE,1> IRQ;
typedef Gpio<GPIOD_BASE,14> redLed;

void invia(char* payload){
    pthread_mutex_lock(&buff);
    //trasmission=true;
    int i=0;
    for(;i<31;i++){
        bufferTransmit[i+counter] = payload[i];
    }
    bufferTransmit[i]='\0';
    counter = counter + 32;
    pthread_mutex_unlock(&buff);
}


void __attribute__((naked)) EXTI1_IRQHandler(){
    saveContext();
    asm volatile("bl _Z16EXTI1HandlerImplv");
    restoreContext();
}

void __attribute__((used))EXTI1HandlerImpl(){
    //NVIC_DisableIRQ(EXTI1_IRQn);
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
    char a[4];
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
         printf("Dammi un stringa da trasmettere\n");
        scanf("%s", a);
        wifi->transmit(4,a);
        printf("Ho trasmesso\n");
        greenLed::low();
        usleep(7000000);
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

void *wifi_receive(void *arg){
    char data[4];
    nRF24L01P *wifi;
    greenLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    wifi = new nRF24L01P();
    wifi->power_up();
    wifi->set_receive_mode();
    configureModuleInterrupt();
    for(;;){
       printf("sto aspettando il carattere\n");
       waitForModule();
       wifi->get_register_status();
        if(wifi->packet_in_pipe(0)){
                 wifi->reset_interrupt();
                 printf("Status register %d\n",wifi->get_register_status());
                 printf("ho ricevuto qualcosa\n");
                 printf("ricevuto da pipe 0 %d\n",wifi->receive(0,data,1));
                 printf("ho ricevuto %s\n",data);
                 redLed::low();
        }
              
    }
    
}  
    void *wifi_transmit(void *arg){
        nRF24L01P *wifi;
        wifi = new nRF24L01P();
        wifi->power_up();
        wifi->set_transmit_mode();
        char payload[32];
        for(;;){
              usleep(5000000);
              pthread_mutex_lock(&buff);
              if(counter == 0){
                   pthread_cond_wait(&cond,&buff); 
              }
              for(int i = 0;i< 31;i++){
                  payload[i]=bufferTransmit[i];
                  
              }
              counter = counter -32;
              pthread_mutex_lock(&spi);
              wifi->transmit(32,payload);
              pthread_mutex_unlock(&spi);
              pthread_mutex_unlock(&buff);
              
            
        }
        
    }

