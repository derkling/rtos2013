
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

    module->setReceiveMode();
    
   
    result =  module->readRegister(0);
    printf("il registro config è: %d\n" , result);
    result =  module->readStatusRegister();
    printf("il registro status è: %d\n" , result);

    
}

