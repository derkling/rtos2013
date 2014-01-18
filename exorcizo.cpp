
#include <cstdio>
#include "miosix.h"
#include "NRF24L01P.h"

using namespace std;
using namespace miosix;

int main()
{
    int result;
    NRF24L01P* module = new NRF24L01P();
    
    module->resetModule();
    
    result = module->readRegister(0);
    printf("<<CONFIG REGISTER>> è: %d\n" , result);
    
    result = module->readRegister(1);
    printf("EN_AA REGISTER è: %d\n" , result);
    
    result = module->readRegister(2);
    printf("EN_RXADDR REGISTER è: %d\n" , result);
    
    result = module->readRegister(3);
    printf("SETUP_AW REGISTER è: %d\n" , result);
    
    result = module->readRegister(4);
    printf("SETUP_RETR REGISTER è: %d\n" , result);
    
    result = module->readRegister(5);
    printf("RF_CH REGISTER è: %d\n" , result);
    
    result = module->readRegister(6);
    printf("RF_SETUP REGISTER è: %d\n" , result);
    
    result = module->readRegister(7);
    printf("<<STATUS REGISTER>> è: %d\n" , result);
    
    result = module->readRegister(8);
    printf("OBSERVE_TX REGISTER è: %d\n" , result);
    
    result = module->readRegister(9);
    printf("RPD REGISTER è: %d\n" , result);
    
    result = module->readRegister(10);
    printf("RX_ADDR_P0 REGISTER è: %d\n" , result);
    
     result = module->readRegister(11);
    printf("RX_ADDR_P1 REGISTER è: %d\n" , result);
    
     result = module->readRegister(12);
    printf("RX_ADDR_P2 REGISTER è: %d\n" , result);
    
     result = module->readRegister(13);
    printf("RX_ADDR_P3 REGISTER è: %d\n" , result);
    
     result = module->readRegister(14);
    printf("RX_ADDR_P4 REGISTER è: %d\n" , result);
    
     result = module->readRegister(15);
    printf("RX_ADDR_P5 REGISTER è: %d\n" , result);
            
     result = module->readRegister(16);
    printf("TX_ADDR REGISTER è: %d\n" , result);
    
    result = module->readRegister(17);
    printf("RX_PW_P0 REGISTER è: %d\n" , result);
    
    result = module->readRegister(18);
    printf("RX_PW_P1 REGISTER è: %d\n" , result);
    
    result = module->readRegister(19);
    printf("RX_PW_P2 REGISTER è: %d\n" , result);
    
    result = module->readRegister(20);
    printf("RX_PW_P3 REGISTER è: %d\n" , result);
   
    result = module->readRegister(21);
    printf("RX_PW_P4 REGISTER è: %d\n" , result);
    
    result = module->readRegister(22);
    printf("RX_PW_P5 REGISTER è: %d\n" , result);
    
    result = module->readRegister(23);
    printf("<<FIFO_STATUS>> REGISTER è: %d\n" , result);
}
