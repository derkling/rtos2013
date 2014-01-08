
#include <cstdio>
#include "miosix.h"
#include "wifi_module.h"


using namespace std;
using namespace miosix;



int main()
{
    pthread_t wifi_thread;
    pthread_create(&wifi_thread,NULL,&wifi_start,NULL);
    pthread_join(wifi_thread,NULL);
}