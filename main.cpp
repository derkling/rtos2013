
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

extern int out_data = 0; //this is a global variable set by podometer thread (podometer must initialize this variable )

extern int in_data = -1; //global variable readed by sound thread and setted by our module 

//ATTENTION: WITH INTEGRATION WE MUST HANDLE THE MUTUAL EXCLUSION TO ACCESS AT THIS GLOBAL VARS 

static pthread_t thread1;

void *beacon(){
    
}

int main(){

    NRF24L01P* module = new NRF24L01P();
    char * pointer = (char*)&out_data;

    module->powerUp();
    module->configureInterrupt();
    module->disableAllAutoAck(); 
    module->setStaticPayloadSize(4); // static payload size 4 byte ( due to the fact that we'll transmit only integer numbers)
   

    while(true) //global cycle of module job
    {   
        module->TrasmitData(pointer, 4);
        
    }
    
    //if something strange happen reset module and powerDown 
    module->resetModule(); 
    module->powerDown();
    
}

