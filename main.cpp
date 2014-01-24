#include "miosix.h"
#include "spi.h"
#include "social_wireless.h"
#include <cstdio>
#include <unistd.h>

using namespace std;
using namespace miosix;

typedef Gpio<GPIOD_BASE,15> blueLed;

int main(){
   
	char payload[33];

	blueLed::mode(Mode::OUTPUT);
       
        init();

        char orangeon[]="orangeon";

        char orangeoff[]="orangeoff";
        
	blueLed::high();

	while(1){
		scanf("%s",payload);
		sendData(payload);		
//		sendData(orangeon);
//		sendData(orangeoff);
//		usleep(50000);
	}
}





