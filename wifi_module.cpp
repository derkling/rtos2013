#include <cstdio>
#include "miosix.h"
#include "spi_driver.h"
#include "nRF24L01P.h"
#include "arch/cortexM4_stm32f4/common/CMSIS/stm32f4xx.h"
#include <miosix/kernel/scheduler/scheduler.h>

#define BUFFER_TRANSMIT_SIZE            96
#define BUFFER_CELL_SIZE                32
#define BUFFER_NUMBER_CELLS             3


using namespace std;
using namespace miosix;


static Thread *waiting=0;
char bufferTransmit[BUFFER_TRANSMIT_SIZE];
int counter = 0;

pthread_mutex_t buff=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t spi=PTHREAD_MUTEX_INITIALIZER;

typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOA_BASE,1> IRQ;
typedef Gpio<GPIOD_BASE,14> redLed;



/**
 * La funzione permette di caricare nel buffer di trasmissione un payload di dimensione
 * 32 in modo che possa essere inviato.
 * @param payload - il payload che deve essere inserito nel buffer
 */
void invia(char* payload){
    pthread_mutex_lock(&buff);
    int i=0;
     
    for(;i<BUFFER_CELL_SIZE;i++){
        bufferTransmit[i+counter] = payload[i];
    }
    
    printf("payload %s\n",payload);
    printf("bufferTrasmit %s\n",bufferTransmit+counter);
    
    counter = counter + BUFFER_CELL_SIZE;
    pthread_mutex_unlock(&buff);
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
    char a[BUFFER_CELL_SIZE];
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
        pthread_mutex_lock(&spi);
        wifi->transmit(BUFFER_CELL_SIZE,a);
        pthread_mutex_unlock(&spi);
        printf("Ho trasmesso\n");
        greenLed::low();
        usleep(7000000);
    } 
}

void *wifi_receive(void *arg){
    char data[BUFFER_CELL_SIZE];
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
       pthread_mutex_lock(&spi);
       wifi->get_register_status();
        if(wifi->packet_in_pipe(0)){
                 wifi->reset_interrupt();
                 printf("Status register %d\n",wifi->get_register_status());
                 printf("ho ricevuto qualcosa\n");
                 printf("ricevuto da pipe 0 %d\n",wifi->receive(0,data,BUFFER_CELL_SIZE));
                 printf("ho ricevuto %s\n",data);
                 redLed::low();
        }
       pthread_mutex_unlock(&spi);
              
    }
    
}  
   
void *wifi_transmit(void *arg){
    nRF24L01P *wifi;
    wifi = new nRF24L01P();
    wifi->power_up();
    wifi->set_transmit_mode();
    char payload[BUFFER_CELL_SIZE];
    for(;;){
        usleep(5000000);
        printf("Mi sono svegliato\n");
        pthread_mutex_lock(&buff);
        if(counter != 0){
            for(int j=0;j<BUFFER_NUMBER_CELLS;j++){
                for(int i = 0;i< BUFFER_CELL_SIZE;i++){
                    payload[i]=bufferTransmit[i+BUFFER_CELL_SIZE*j];
                }
                printf("payload %s\n",payload);
                printf("bufferTrasmit %s\n",bufferTransmit+BUFFER_CELL_SIZE*j);
                pthread_mutex_lock(&spi);
                wifi->transmit(BUFFER_CELL_SIZE,payload);
                pthread_mutex_unlock(&spi);
            }
            counter = 0;
        }
        pthread_mutex_unlock(&buff);
    }      
}

