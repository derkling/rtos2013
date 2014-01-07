
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include "miosix.h"
#include "spi.h"

using namespace std;
using namespace miosix;

int main()
{
	uint8_t sr;
	configureSpi();
	while(1){
   		chipEnable();
		spiSendCommandWriteData(NOP,COMMAND_WITHOUT_ADDRESS,&sr,NULL,0);
		chipDisable();
	}
}

