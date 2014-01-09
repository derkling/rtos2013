#include <cstdio>
#include "miosix.h"
#include "spi_driver.h"
#include "nRF24L01P.h"

using namespace std;
using namespace miosix;

bool trasmission;
int num_step=0;

void invia(int num_passi){
    trasmission=true;
}

void *wifi_start(void *arg)
{
    nRF24L01P *wifi;
    wifi = new nRF24L01P();
    
    wifi->init();
    
    for(;;){
        if (trasmission){
            wifi->transmit(num_step);
        }
        else{
            wifi->receive();
        }
    
    }
   
    printf("Hello world, write your application here\n");
    
}