#include "miosix.h"
#include <cstdio>
#include <unistd.h>
#include "queue.h"

using namespace std;
using namespace miosix;

typedef Gpio<GPIOD_BASE,14> red_led;
typedef Gpio<GPIOD_BASE,12> green_led;

int main(){
    
	red_led::mode(Mode::OUTPUT);
        green_led::mode(Mode::OUTPUT);

        char payload[]="qwerty";
        queue_t queue;
        queueInizializer(&queue);
        int result = 0;
        int push = 0;
	while(result == 0){
            result = queuePush(payload, &queue);
            push++;
	}
        printf("\n");
        char str[32];
        int pop = 0;
        while(!queueIsEmpty(&queue)){
            queuePop(&queue, str);
            pop++;
        }
        printf(" ");
}





