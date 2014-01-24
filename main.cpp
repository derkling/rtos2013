#include "miosix.h"
#include <cstdio>
#include <unistd.h>
#include "queue.h"

using namespace std;
using namespace miosix;

//typedef Gpio<GPIOD_BASE,14> red_led;
//typedef Gpio<GPIOD_BASE,12> green_led;

int main(){
    
//	red_led::mode(Mode::OUTPUT);
//        green_led::mode(Mode::OUTPUT);

        char payload[]="qwerty";
        queue_t queue;
        queueInizializer(&queue);
        int result;
        char str[33];
        int push = 0;
        int pop = 0;
        for (int k = 0; k < 300; k++){
            for (int i = 0; i < 2; i++){
                result = queuePush(payload, &queue);
                if (result == 0)
                    push++;
            }
            
            if (!queueIsEmpty(&queue)){
                queuePop(&queue, str);
                pop++;
            }
        }
        printf(" ");
}





