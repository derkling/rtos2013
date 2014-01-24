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

        char orangeon[]="orangeon";

        char orangeoff[]="orangeoff";
        
        char beep[]="beep";
        
	
	blueLed::high();
//        beep();

	while(1){
		
		sendData(orangeon);
		sendData(orangeoff);
//		sendData(beep);
		sleep(2);
	}
}





