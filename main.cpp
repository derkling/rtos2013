#include <cstdio>
#include "miosix.h"
#include <unistd.h>
#include <cstring>
#include <map>
#include "Proprieta.h"
#include "Comando.h"

using namespace std;
using namespace miosix;

//PD14: red led
Comando comando;
Proprieta proprieta;

int main(){
        char primaStringa[30];
        char secondaStringa[30];
        float valore=-1;
        char msgComandoErrato[50] = "il comando inserito non esiste.";
        char msgProprietaErrata[50] = "la proprieta inserita non esiste.";
        char msgValoreErrato[50] = "il valore inserito non è valido.";
    
        printf("####################################################### \n");
        printf("##         elenco con sintassi dei comandi :         ## \n");
        printf("##                                                   ## \n");
        printf("##          read          proprieta                  ## \n");
        printf("##          update        proprieta     valore       ## \n");
        printf("##          deposit       proprieta     valore       ## \n");
        printf("##          subscribe     modulo        proprieta    ## \n");
        printf("##          check         modulo                     ## \n");
        printf("##          notify        proprieta1    proprieta2   ## \n");
        printf("####################################################### \n");
        
        for(;;){
            scanf("%s",primaStringa);
            if (comando.esisteComando(primaStringa)){
                        if(strcmp(primaStringa,"read")){
                            scanf("%s%*[^\n]",secondaStringa);
                            if(proprieta.esisteProprieta(secondaStringa)){
                                printf("%s = %d \n", secondaStringa, comando.read(secondaStringa));
                            }
                            else 
                                printf("%s \n", msgProprietaErrata);
                        }
                        if(strcmp(primaStringa,"update")){
                            scanf("%s",secondaStringa);
                            if(proprieta.esisteProprieta(secondaStringa)){
                                while(valore==-1) {
                                    try{
                                            scanf("%f%*[^\n]",&valore);
                                            if(valore<0){
                                                printf("il valore immesso non è un numero positivo!\n");
                                                printf("immettere un valore : ");
                                            }
                                    }
                                    catch (...) {
                                        printf("il valore immesso non è un numero! \n");
                                        printf("immettere un valore : ");
                                    }
                                }
                                comando.update(secondaStringa, valore);
                                printf("proprieta %s aggiornata con successo!! \n", secondaStringa);
                            }
                            else 
                                printf("%s \n", msgProprietaErrata);
                        }
            }
            else
                    printf("%s \n", msgComandoErrato);
        }

}
/*
            if(strcmp(primaStringa,"get")==0){
                scanf("%s%*[^\n]",secondaStringa);
            }
            else {
                scanf("%*[^\n]");
            }
 */