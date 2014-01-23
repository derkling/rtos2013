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
       
        init();

        char payload[]="orangeon";
        sendData(payload);

        char payload3[]="orangeoff";
        sendData(payload3);
	
	blueLed::high();
//        beep();

	while(1){
        	sendData(payload);
		usleep(100000);
        	sendData(payload3);
		usleep(100000);
	}
}





