
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"



using namespace std;
using namespace miosix;

int main()
{
    int result;
    NRF24L01P* module = new NRF24L01P();
    
    module->powerUp();
    module->configureInterrupt();
    module->writeRegister(1,0); //disabilito su tutti i canali l'auto ack ( TEMPORANEO )

   //module->setRfChannel(2);
    //module->setAirDataRate(38); // if this is 38, the transmitter must be 38 too
    
    module->writeRegister(17,4); // setto la grandezza del payload da ricevere a 1 byte!  STATIC PAYLOAD 
    module->setReceiveMode();
    
    
    
    while(1)
    {
      while(!(module->readStatusRegister() & 0x40 )) //0x40 is 64 in decimal, that correspond to RX_DS
           {

          printf("%d", module->readRPD());

          }
    char data = (char) module->receiveDataFromRx();
    module->showInternal();
    printf("Ho ricevuto %c\n",data);
    sleep(10);
    
    }
    
    
}

