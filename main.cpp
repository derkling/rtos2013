
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;


int main(){

    NRF24L01P* module = new NRF24L01P();
    int what_is_rpd=0;
    char data = 'a';
    char* pointer = &data;
    
    module->powerUp();
    module->configureInterrupt();
    module->writeRegister(1,0); //disabilito su tutti i canali l'auto ack ( TEMPORANEO )
    
    module->writeRegister(17,1); // setto la grandezza del payload da ricevere a 1 byte!  STATIC PAYLOAD 
    module->setReceiveMode();
    
    while(true)
    {
     char temp = module->readData();
     if(temp!='')
        {
          // gestisci la ricezione di qualcosa SAD trombone / ecc..
        }
     sleep(500);
     what_is_rpd = module->readRPD();
     if(what_is_rpd == 1 ) //If there is somebody near me and there are data to send!
        {
         module->TrasmitData(pointer , 1); //This functions take the module first to receive -> transmit and then transmit -> receive
        }
    }
    
}

