
#include "player.h"
#include "prova_16.h"


#include "sad_trombone.h"

void delay()
{
	volatile int i;
	for(i=0;i<1000000;i++) ;
}

int main()
{
    
        //ADPCMSound sound2(num_441_bin,num_441_bin_len);
        ADPCMSound sound2(prova_16_bin,prova_16_bin_len);
	Player::instance().play(sound2, -20);
}
