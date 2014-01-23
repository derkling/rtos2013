
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

int main()
{
    NRF24L01P* module = new NRF24L01P();
    int result; char data='f';
    char* pointer;
    
    
    module->powerUp();
    module->configureInterrupt();
    
    module->writeRegister(1,0); //disabilito su tutti i canali l'auto ack ( TEMPORANEO )
    //module->setAirDataRate(38);
    //module->setRfChannel(2);
    
    
    /*DEBUGGING THE TRASMISSION*/
    
    
    module->showInternal();
    
    //printf("Dammi un carattere da trasmettere\n");
    //scanf("%c", &data);
    pointer = &data;
    
    while(1){
    module->TrasmitData(pointer , 1);
    usleep(10000);
    }
    
    
   /*
    result = module->readRegister(3);
    printf("il registro setup_aw prima di aver trasmesso è: %d\n" , result);
    
    result = module->readRegister(10);
    printf("il registro TX_ADDR prima di aver trasmesso è: %d\n" , result);
    
    
    
    
 
    result =  module->readRegister(0);
    printf("il registro config è: %d\n" , result);
    
   
    sleep(5);
    
     result =  module->readStatusRegister();
    printf("il registro status dopo aver trasmesso è: %d\n" , result);
    
    result = module->readRegister(23);
    printf("il registro FIFO_STATUS_TX dopo aver trasmesso è: %d\n" , result);
    */
}
