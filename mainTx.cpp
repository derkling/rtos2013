
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

int main()
{
    NRF24L01P* module = new NRF24L01P();
    
    module->powerUp();
    module->sendDataToTx(12);
    module->setTransmitMode();
     
}
