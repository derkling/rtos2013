
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

    module->setReceiveMode();
    module->setRfChannel(2);
    module->setAirDataRate(38); // if this is 38, the transmitter must be 38 too
    
    module->showInternal();

    
}

