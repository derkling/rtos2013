
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

extern int out_data = 0; //this is a global variable set by podometer thread (podometer must initialize this variable )

extern int in_data = -1; //global variable readed by sound thread and setted by our module 

//ATTENTION: WITH INTEGRATION WE MUST HANDLE THE MUTUAL EXCLUSION TO ACCESS AT THIS GLOBAL VARS 

int main(){

    NRF24L01P* module = new NRF24L01P();
    char * pointer = (char*)&out_data;

    module->powerUp();
    module->configureInterrupt();
    module->disableAllAutoAck(); 
    module->setStaticPayloadSize(4); // static payload size 4 byte ( due to the fact that we'll transmit only integer numbers)
    module->setReceiveMode();

    while(true) //global cycle of module job
    {
     while(module->readRPD()==0 && ( ( module->readStatusRegister() & 0x40 ) == 0 ) )  //untill RPD is 0 and there are no RX_DR pending
          {/*w8 something...*/
            printf("[INFO_MESSAGE]: waiting for events\n");  //debug
          }
     if(module->readRPD() != 0 ) //Somebody near me!
       {
         printf("[INFO_MESSAGE]: transmission");
         module->TrasmitData(pointer,4); //the reset of tx_ds irq is inside the function
         module->notifyTX();
       }
     else //Something received
         {
         printf("[INFO_MESSAGE]: receiving");
         in_data = module->receiveDataFromRx();
         module->resetRXirq();
         module->notifyRX();
         }
    }
    
    //if something strange happen reset module and powerDown 
    module->resetModule(); 
    module->powerDown();
    
}

