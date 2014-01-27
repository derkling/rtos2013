#include <cstdio>
#include "miosix.h"
#include "spi_driver.h"
#include "nRF24L01P.h"

#include <miosix/kernel/scheduler/scheduler.h>

#define BUFFER_TRANSMIT_SIZE            96
#define BUFFER_CELL_SIZE                32
#define BUFFER_NUMBER_CELLS             3
#define BUFFER_RECEIVE_SIZE             96

using namespace std;
using namespace miosix;


static Thread *waiting=0;
static char buffer_transmit[BUFFER_TRANSMIT_SIZE];
static char buffer_receive[BUFFER_RECEIVE_SIZE];
static int counter_tx = 0;
static int counter_rx = 0;
static nRF24L01P *wifi;
static pthread_mutex_t buff_tx=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t spi=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t buff_rx=PTHREAD_MUTEX_INITIALIZER;

typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOA_BASE,1> IRQ;
typedef Gpio<GPIOD_BASE,14> redLed;
typedef Gpio<GPIOD_BASE,15> blueLed;
typedef Gpio<GPIOD_BASE,13> orangeLed;


/**
 * La funzione permette di caricare nel buffer di trasmissione un payload di dimensione
 * 32 in modo che possa essere inviato.
 * @param payload - il payload che deve essere inserito nel buffer
 */
void invia(char* payload){
    pthread_mutex_lock(&buff_tx);
    int i=0;
    if(counter_tx == BUFFER_TRANSMIT_SIZE){
        printf("Buffer transmit pieno\n");
        pthread_mutex_unlock(&buff_tx);
        return;
    }
    for(;i<BUFFER_CELL_SIZE-1;i++){
        buffer_transmit[i+counter_tx] = payload[i];
    }
    buffer_transmit[i+counter_tx]='\0';
    //printf("payload %s\n",payload);
    //printf("bufferTrasmit %s\n",buffer_transmit+counter_tx);
    
    counter_tx = counter_tx + BUFFER_CELL_SIZE;
    pthread_mutex_unlock(&buff_tx);
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
    /*nRF24L01P *wifi;
    wifi = new nRF24L01P();
    greenLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    configureModuleInterrupt();*/
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

/**
 * Il metodo costituisce la funzione che verrà svolta dal thread per la ricezione
 * @param arg
 * @return void
 */
void *wifi_receive(void *arg){
    char data[BUFFER_CELL_SIZE];
    /*nRF24L01P *wifi;
    orangeLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    blueLed::mode(Mode::OUTPUT);
    wifi = new nRF24L01P();
    wifi->power_up();
    wifi->set_receive_mode();*/
    blueLed::high();
    //configureModuleInterrupt();
    for(;;){
       printf("sto aspettando il carattere\n");
       orangeLed::high();
       waitForModule();
       pthread_mutex_lock(&spi);
       wifi->set_receive_mode();
       wifi->get_register_status();
       if(wifi->packet_in_pipe(0)){
                 orangeLed::low();
                 wifi->reset_interrupt();
                 //printf("Status register %d\n",wifi->get_register_status());
                 //printf("ho ricevuto qualcosa\n");
                 printf("ricevuto da pipe 0 %d\n",wifi->receive(0,data,BUFFER_CELL_SIZE));//mettere controllo su altro da leggere
                 printf("ho ricevuto %s\n",data);
                 pthread_mutex_lock(&buff_rx);
                 for(int i=0;i<BUFFER_CELL_SIZE;i++){
                     buffer_receive[i+counter_rx] = data[i];
                 }
                 counter_rx += BUFFER_CELL_SIZE;
                 pthread_mutex_unlock(&buff_rx);
                 redLed::low();
       }
       pthread_mutex_unlock(&spi);
              
    }
    
}  
/**
 * Il metodo implementa la funzione che viene svolta dal thread di trasmissione. Ogni 5 secondi verifica
 * se nel buffer è presente qualcosa da trasmettere: se è così lo svuota completamente.
 * @param arg
 * @return 
 */
void *wifi_transmit(void *arg){
    /*nRF24L01P *wifi;
    wifi = new nRF24L01P();
    greenLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    blueLed::mode(Mode::OUTPUT);
    wifi->power_up();
    wifi->set_transmit_mode();*/
    blueLed::high();
    char payload[BUFFER_CELL_SIZE];
    for(;;){
        greenLed::high();
        usleep(5000000);
        //printf("Mi sono svegliato\n");
        pthread_mutex_lock(&buff_tx);
        if(counter_tx != 0){
            for(int j=0;j<counter_tx/BUFFER_CELL_SIZE;j++){
                for(int i = 0;i< BUFFER_CELL_SIZE;i++){
                    payload[i]=buffer_transmit[i+BUFFER_CELL_SIZE*j];
                    buffer_transmit[i+BUFFER_CELL_SIZE*j] = 0;
                }
                //printf("payload %s\n",payload);
                //printf("bufferTrasmit %s\n",buffer_transmit+BUFFER_CELL_SIZE*j);
                pthread_mutex_lock(&spi);
                wifi->transmit(BUFFER_CELL_SIZE,payload);
                pthread_mutex_unlock(&spi);
                greenLed::low();
                usleep(400000);
                greenLed::high();
            }
            counter_tx = 0;
           
        }
        pthread_mutex_unlock(&buff_tx);
    }      
}

void ricevi(char *payload){
    pthread_mutex_lock(&buff_rx);
    if(counter_rx == 0){
        pthread_mutex_unlock(&buff_rx);
        printf("Non ho nulla da darti\n");
        return;
    }
    for(int i=0;i<counter_rx/BUFFER_CELL_SIZE;i++){
        for(int j=0;j<BUFFER_CELL_SIZE;j++){
            payload[j] = buffer_receive[j+BUFFER_CELL_SIZE*i];
            buffer_receive[j+counter_rx*i] = 0;
        }
    }
    counter_rx=0;
    pthread_mutex_unlock(&buff_rx);
}

void init(){
    orangeLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    blueLed::mode(Mode::OUTPUT);
    greenLed::mode(Mode::OUTPUT);
    wifi = new nRF24L01P();
    wifi->power_up();
    wifi->set_receive_mode();
    configureModuleInterrupt();
}
