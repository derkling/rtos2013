
#include <cstdio>
#include "miosix.h"
#include "wifi_module.h"


using namespace std;
using namespace miosix;



int main()
{
    char a[32];
    pthread_t wifi_thread;
    pthread_t thread_receive;
    pthread_create(&wifi_thread,NULL,&wifi_transmit,NULL);
    pthread_create(&thread_receive,NULL,&wifi_receive,NULL);
    while(1){
    printf("Dammi un stringa da trasmettere\n");
        scanf("%s", a);
        invia(a);
    }
    pthread_join(wifi_thread,NULL);
    pthread_join(thread_receive,NULL);
    
       
}