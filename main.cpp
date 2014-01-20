#include "miosix.h"
#include "spi.h"
#include "social_wireless.h"
#include <cstdio>
#include <unistd.h>

using namespace std;
using namespace miosix;

typedef Gpio<GPIOD_BASE,15> blueLed;

int main(){
    
	blueLed::mode(Mode::OUTPUT);
	
     
	uint8_t sr=255;
        
        uint8_t data;
       
        init();

//      beep();
        char* payload="orange on";
        sendData(payload);
        
        payload="beep";
        sendData(payload);
        
        payload="orange off";
        sendData(payload);
        
        payload="orange on";
        sendData(payload);
        
	
//        spiSendCommandReadData(R_REGISTER,OBSERVE_TX,&sr,&data,1);
	
	blueLed::high();
//        beep();

	while(1){

	}
}





