
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

int main()
{
    NRF24L01P* module = new NRF24L01P();
    int result; char data='f', data2='a', data3='b' , data4='i' , data5='o';
    char* pointer;
    char* pointer2;
    char* pointer3;
    char* pointer4;
    char* pointer5;
    
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
    pointer2 = &data2;
    pointer3 = &data3;
    pointer4= &data4;
    pointer5 = &data5;
    while(1){
    module->TrasmitData(pointer , 1);
    usleep(1000);
    module->TrasmitData(pointer2,1);
    usleep(1000);
    module->TrasmitData(pointer3,1);
    usleep(1000);
    module->TrasmitData(pointer4,1);
    usleep(1000);
    module->TrasmitData(pointer5,1);
    usleep(1000);
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
