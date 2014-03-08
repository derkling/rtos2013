
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

int out_data = 91; //this is a global variable set by podometer thread 

int in_data = 0; //global variable readed by sound thread 


int main(){

    NRF24L01P* module = new NRF24L01P();
    char * pointer = (char*)&out_data;
    
    module->powerUp();
    module->configureInterrupt();
    module->writeRegister(1,0); //disable all the auto-ack 
    
    module->writeRegister(17,4); // 4 bytes of static payload
    module->setReceiveMode();
    
    while(true) //global cycle of module job
    {
     while(module->readRPD()==0 && ( ( module->readStatusRegister() & 0x40 ) == 0 ) )  //untill RPD is 0 and there are no RX_DR pending
          {/*w8 something...*/
            printf("w8 for something\n");  //debug
          }
     if(module->readRPD() != 0 ) //Somebody near me!
       {
         module->TrasmitData(pointer,4); //the reset of tx_ds irq is inside the function
       }
     else //Something received
         {
         in_data = module->receiveDataFromRx();
         module->resetRXirq();
         }
    }
    
}

