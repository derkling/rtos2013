
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
	uint8_t config_reg;
	uint8_t rilettura;
	configureSpi();
	powerLineUp();
	sleep(1);
	while(1){
		spiSendCommandReadData(R_REGISTER,CONFIG,&sr,&config_reg,1);
		sleep(1);
		config_reg |= 1;
		spiSendCommandWriteData(W_REGISTER,CONFIG,&sr,&config_reg,1);
		sleep(1);
		spiSendCommandReadData(R_REGISTER,CONFIG,&sr,&rilettura,1);
		sleep(1);
	}
}

