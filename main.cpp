
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include "miosix.h"

using namespace std;
using namespace miosix;

typedef Gpio<GPIOD_BASE,15> blueLed;
typedef Gpio<GPIOD_BASE,12> greenLed;
typedef Gpio<GPIOA_BASE,0> blueButton;
typedef Gpio<GPIOB_BASE,1> buzzer;

void *do_work(void *args);

int main()
{
    //iprintf("Hello world, write your application here\n");
    blueLed::mode(Mode::OUTPUT);
    blueButton::mode(Mode::INPUT);
    buzzer::mode(Mode::OUTPUT);
    pthread_t worker_thread;
    int stato=0;
    int prev=0;
//  int pthread_create(pthread_t *worker_thread, pthread_attr_t *attr, void *(*start_routine) (void*), void *arg); 
    if (pthread_create(&worker_thread, NULL, do_work, NULL)){
        printf("Error while creating thread\n");
        exit(1);
    } 
    for(;;){
        
        while(blueButton::value()==1 && prev==0){
            if(stato==1){
                blueLed::high();
                stato=0;
                
            }
            else {
                blueLed::low();
                stato=1;
                
            }
            prev=1;
            buzzer::high();
            usleep(100000);//non permette il cambio dello stato troppo veloce
            buzzer::low();
            while(blueButton::value()==1){}
        }
        
        prev=0;
//        while(blueButton::value()==0){}
        
    }
    
}

void *do_work(void *args){
    for(;;){
        char var;
        scanf("%c",&var);
        greenLed::mode(Mode::OUTPUT);
        if(var=='g')greenLed::low();
        else if(var=='G')greenLed::high();
    }
    

}

