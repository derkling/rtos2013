
#include <cstdio>
#include "miosix.h"
#include "wifi_module.h"


using namespace std;
using namespace miosix;



int main()
{
    char transmit[32];
    char receive[96];
    pthread_t wifi_thread;
    pthread_t thread_receive;
    init();
    pthread_create(&wifi_thread,NULL,&wifi_transmit,NULL);
    pthread_create(&thread_receive,NULL,&wifi_receive,NULL);
    while(1){
    printf("Dammi un stringa da trasmettere\n");
        scanf("%s", transmit);
        invia(transmit);
        ricevi(receive);
        printf("Ho ricevuto nel main %s\n",receive);
    }
    pthread_join(wifi_thread,NULL);
    pthread_join(thread_receive,NULL);
    
       
}