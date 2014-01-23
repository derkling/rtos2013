#include "miosix.h"
#include "spi.h"
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"

void beep(void);

void transmit(char* payload);

using namespace std;
using namespace miosix;

//gpio
typedef Gpio<GPIOB_BASE,1> buzzer;
typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOD_BASE,13> orangeLed;
typedef Gpio<GPIOD_BASE,14> redLed;

//var globali
static uint8_t rxPayload[MAX_LENGHT_PAYLOAD+1]={0};

static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
static pthread_mutex_t str=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t spi=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
static queue_t queue;


static pthread_t interrupt_thread;
static pthread_t transmit_thread;
static bool interrupt=false;


//INTERRUPT

void __attribute__((naked)) EXTI1_IRQHandler(){

	saveContext();
	asm volatile("bl _Z16EXTI1HandlerImplv");
	restoreContext();

}

/*!@brief routine di interrupt scatenato dal modulo wireless
 * verrà chiamato quando arriva un pacchetto o finisce la trasmissione
 * called from hardware, producer, modifica la condition variable interrupt
 */
void __attribute__((used)) EXTI1HandlerImpl(){
    
    NVIC_DisableIRQ(EXTI1_IRQn);
    EXTI->PR = EXTI_PR_PR1;//era usato uguale (=) boh    
	pthread_mutex_lock(&mutex);
	interrupt=true;
    pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
    greenLed::high();
}

/*!@brief Funzione che rappresenta il thread di trasmissione
 * @note Il thread non termina mai. Si sveglia ogni 5 secondi e se c'è un pacchetto in coda lo trasmette
 */
void *transmitConsumer(void *arg){
	char payload[MAX_LENGHT_PAYLOAD+1];
    for(;;){
        sleep(5);

        pthread_mutex_lock(&str);


        if(!queueIsEmpty(&queue)){

            
            queuePop(&queue,payload);
            pthread_mutex_unlock(&str);
            pthread_mutex_lock(&spi);
            transmit(payload);
            pthread_mutex_unlock(&spi);
            
            
        }
        pthread_mutex_unlock(&str);
    
    }
    

}

void *interruptConsumer(void *arg){
    
    
    
    for(;;){
        
        pthread_mutex_lock(&mutex);
        while(!interrupt)pthread_cond_wait(&cond,&mutex); //questo thread è in wait fino a che la var interrupt non diventa true
        interrupt=false;
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&spi);
        //mutex spi locked
        
        //gestione interrupt
        
        uint8_t sr;
        uint8_t reset=0x70;//0b1110000 reset the RX_DR, TX_DS, MAX_RT
        spiSendCommandWriteData(W_REGISTER,STATUS,&sr,&reset,1);
        
        sr=sr&0b01110000;//lo status register prima di effettuare il reset

        if(sr==32){
            //tx interrupt
//            beep();
            
        }
        else if(sr==64 || sr==96 ){
            //rx interrupt
            redLed::high();
//            beep();
            
            uint8_t payloadWidth;
            
            spiSendCommandReadData(R_RX_PL_WID,COMMAND_WITHOUT_ADDRESS,&sr,&payloadWidth,1);
            
            
            
            spiSendCommandReadData(R_RX_PAYLOAD,COMMAND_WITHOUT_ADDRESS,&sr,rxPayload,(int)payloadWidth);
            rxPayload[payloadWidth]='\0';
            
//          printf("%s",rxPayload);
//          if(payload_sender==podometro)podometro.messageforyou
            if(strncmp((char*)rxPayload,"orangeon",payloadWidth)==0)orangeLed::high();
            if(strncmp((char*)rxPayload,"orangeoff",payloadWidth)==0)orangeLed::low();
            if(strncmp((char*)rxPayload,"beep",payloadWidth)==0)beep();
  
        }

        //000 0 none (impossibile)
        //001 16 max
        //010 32 tx
        //011 48 tx-max
        //100 64 rx
        //101 80 rx-max
        //110 96 rx tx
        //111 112 rx tx max    
        
        NVIC_EnableIRQ(EXTI1_IRQn);
        
        pthread_mutex_unlock(&spi);
        
    }
}


/*!@brief Inizializza il modulo NRF24LR: configura SPI2, alloca la coda di trasmissione, crea il thread di trasmissione e il thread interrupt
 */
void init(){
    
    buzzer::mode(Mode::OUTPUT);
    greenLed::mode(Mode::OUTPUT);
    orangeLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    
    queueInizializer(&queue);
    
    configureSpi();//0.65mbps, frame 8 bit, msbit first, lsbyte firts, ss software, stm32 master
    powerLineDown();//VDD=0V
    usleep(1000);//tempo simbolico per far spegnere il modulo rf        
    powerLineUp();//VDD=3V
    usleep(11000);//quando VDD>1.9V serve un t > 10.5ms per entrare nello stato POWER DOWN
    //in POWER DOWN è possibile configurare tutti i registri attraverso l'spi

    //inizio configurazione modulo rf. alla fine della configurazine il modulo rf sarà in RX MODE
    //ottimizzazione generale: dove possibile vengono struttati i valori di defaul dei registri
    uint8_t sr;
    uint8_t data;

    //config canale rf. il canale è lasciato quello di default (2400Mhz+2*1MHz)
    //rf_ch: 0x02 (default)

    //config air data rate
    data=RF_SETUP_1MBPS;
    spiSendCommandWriteData(W_REGISTER,RF_SETUP,&sr,&data,1);//rf_setup=0b00000111 (000,force pll lock,air data=1mbps,tx power=0dbm,lna gain=enabled)

    //config nrf shockburst: dynamic paylod, auto ack enabled, arc=0 (auto retx count)
    //DYNPD->DPL_P0 requires FEATURE->EN_DPL and EN_AA->ENAA_P0 (a quanto pare con dynamic payload non possiamo disattivare l'auto ack)

    //ACTIVATE command followed by 0x73 required by DYNPD and FEATURE
    data=ACTIVATE_BYTE;
    spiSendCommandWriteData(W_REGISTER,ACTIVATE,&sr,&data,1);

    //FEATURE->EN_DPL=1 FEATURE->EN_DYN_ACK=1
    //FEATURE=0x05
    data=FEATURE_EN_DPL;
    spiSendCommandWriteData(W_REGISTER,FEATURE,&sr,&data,1);//feature(1d)=0b00000101 (00000,en_dpl,en_ack_pay,en_dyn_ack)

    //EN_AA->ENAA_P0=1
    //EN_AA=0x01
    data=EN_AA_ENAA_P0;
    spiSendCommandWriteData(W_REGISTER,EN_AA,&sr,&data,1);
    //da provare se questa è compatibile con payload dinamico
    //no auto ack: en_aa=0b00000000 (0,0,data pipe 5 ... data pipe 0)

    //DYNPD->DPL_P0=1
    //DYNPD=0x01
    data=DYNPD_DPL_P0;
    spiSendCommandWriteData(W_REGISTER,DYNPD,&sr,&data,1);//dynpd(1c)=0b00000001 (00,data pipe 5 ... data pipe 0)

    //SETUP_RETR->ARC=0
    //SETUP_RETR=0x00
    data=SETUP_RETR_ARC0;
    spiSendCommandWriteData(W_REGISTER,SETUP_RETR,&sr,&data,1);//auto retx count=0: imposta il numero di auto ritrasmissioni a zero

    //EN_RXADDR->ERX_P0=1
    //EN_RXADDR=0x01
    //attiva solo la data pipe 0 in ricezione, cioè l'unico indirizzo valido per la ricezione sarà quello di data pipe 0 (e7e7e7e7e7)
    //default: sono attive data pipe 0 (e7e7e7e7e7) e data pipe 1 (c2c2c2c2c2)
    //questo comando serve per spegnere data pipe 1 perchè non necessaria (ottimizzazione)
    data=EN_RXADDR_ERX_P0;
    spiSendCommandWriteData(W_REGISTER,EN_RXADDR,&sr,&data,1);//only data pipe 0: en_rxaddr=0b00000001 (00,data pipe 5 ... data pipe 0)

    //conviene sfruttare gli indirizzi rx e tx di default
    //indirizzo rx: e7e7e7e7e7
    //indirizzo tx: e7e7e7e7e7

    //CONFIG->PWR_UP=1 CONFIG->PRIM_RX=1
    //CONFIG=0x0B
    data=CONFIG_PWRUP_PRIM_RX;
    spiSendCommandWriteData(W_REGISTER,CONFIG,&sr,&data,1);//power up, rx mode: write 0b0001011 in config (0, mask rxdr, mask txds, mask maxrt, encrc, crco=1byte, pwr_up, prim_rx)
    usleep(2000);//quando PWR_UP diventa 1 serve un t > 1.5ms per entrare nello stato STBY1 (Standby 1)

    chipEnable();
    usleep(150);//t>130us per raggiungere lo stato RX MODE. si resta in questo stato finchè CE=1 e CONFIG->PWR_UP=1

    //a questo punto il modulo è in RX MODE
    
    pthread_create(&interrupt_thread,NULL,&interruptConsumer,NULL);
    
    pthread_create(&transmit_thread,NULL,&transmitConsumer,NULL);
    
}

/*!@brief Trasmissione immediata della stringa passata come parametro
 * @param payload: stringa da trasmettere
 * @note La funzione deve essere chiamata quando il modulo NRF24LR è in stato di ricezione
 */
void transmit(char* payload){
    
    //RX MODE
    
    chipDisable();
    
    //STBY1 MODE
    
    uint8_t sr;
    uint8_t data;

    //CONFIG->PWR_UP=1 CONFIG->PRIM_TX=0
    //CONFIG=0A
    data=CONFIG_PWRUP_PRIM_TX;
    spiSendCommandWriteData(W_REGISTER,CONFIG,&sr,&data,1);//power up, tx mode: write 0b00001010 in config (0, mask rxdr, mask txds, mask maxrt, encrc, crco=1byte, pwr_up, prim_rx)
    
    //copia i dati in tx_fifo
    spiSendCommandWriteData(W_TX_PAYLOAD_NOACK,COMMAND_WITHOUT_ADDRESS,&sr,(uint8_t*)payload,strlen(payload));
    
    //chipenable for t > 10us to send one packet
    chipEnable();
    usleep(15);
    chipDisable();
    usleep(130);
    
    //TX MODE
    
    usleep(2000);//aspettare almeno 1ms per trasmettere un pacchetto
    
    //STBY1 MODE OR STBY2 MODE
    
    //CONFIG->PWR_UP=0 CONFIG->PRIM_RX=1
    //CONFIG=09
    data=CONFIG_PWRDOWN_PRIM_RX;
    spiSendCommandWriteData(W_REGISTER,CONFIG,&sr,&data,1);//power down, rx mode: write 0b00001001 in config (0, mask rxdr, mask txds, mask maxrt, encrc, crco=1byte, pwr_up, prim_rx)
    
    //POWER DOWN MODE
    
    //CONFIG->PWR_UP=1 CONFIG->PRIM_RX=1
    //CONFIG=0x0B
    data=CONFIG_PWRUP_PRIM_RX;
    spiSendCommandWriteData(W_REGISTER,CONFIG,&sr,&data,1);//power up, rx mode: write 0b0001011 in config (0, mask rxdr, mask txds, mask maxrt, encrc, crco=1byte, pwr_up, prim_rx)
    usleep(2000);//quando PWR_UP diventa 1 serve un t > 1.5ms per entrare nello stato STBY1 (Standby 1)

    chipEnable();
    usleep(150);//t>130us per raggiungere lo stato RX MODE. si resta in questo stato finchè CE=1 e CONFIG->PWR_UP=1

    //a questo punto il modulo è in RX MODE
    
    
        
}


/*!@brief Trasmissione via radio della stringa passata come parametro
 * @param payload: stringa da trasmettere
 * @retval Ritorna -1 se c'è stato un errore, 0 altrimenti
 * @note La trasmissione non è istantanea. Il payload è inserito in una coda e trasmesso appena possibile.
 * @note La funzione si blocca fino a quando non ottiene il permesso di scrittura in coda
 */
int sendData(char* payload){
    pthread_mutex_lock(&str);
    int res=queuePush(payload, &queue);
    pthread_mutex_unlock(&str);
    return res;
}

void beep(){
    buzzer::high();
    sleep(1);
    buzzer::low();
    sleep(1);
}

