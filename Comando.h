/* 
 * File:   Comando.cpp
 * Author: Liscio-Cazzetta
 * 
 * la classe Comando.cpp gestisce i comandi presenti nel vector comandi, quindi
 * in base all'input dell'utente restituisce un valore di una proprieta passata
 * per input oppure valori booleani per il controllo di correttezza di input
 */

#ifndef COMANDO_H
#define COMANDO_H

#include <cstdio>
#include "miosix.h"
#include <unistd.h>
#include <cstring>
#include <map>
#include "Proprieta.h"

using namespace std;

class Comando
{
public:
        struct tipo_Comandi {
            char comando[10];
        };
        tipo_Comandi comandi[6];
        
        /* inizializza la lista dei comandi
         */
	Comando();

        /* data una stringa, verifica che questa corrisponda a qualche comando
         * presente nella lista dei comandi; se si, ritorna true, altrimenti false
         */
        bool esisteComando(const char *stringa);
        
        int read(const char *nomeProprieta);
        
	void update(const char *nomeProprieta, int valore);
        
        /* un modulo ha l'opportunita di inserire una nuova proprieta nelle'elenco
         */
	void store(const char *nomeProprieta, int valore);
        
        /* un modulo puo iscriversi ad una certa proprieta per ricevere una notifica 
         * ogni qualvolta tale proprieta si aggiorna
         */
	void subscribe(const char *nomeModulo, const char *nomeProprieta);
        
        /* questo metodo permette di verificare se ci sono aggiornamenti alle proprieta
         * ogni qualvolta tale proprieta si aggiorna
         */
        bool check(const char *nomeModulo);
        
        /* funzione che aggiorna altre proprieta in seguito all'aggiornamento di una
         * proprieta rilevante che aveva memorizzate le callback function delle altre
         * proprieta
         */
        void notify(const char *nomeProprieta, int valore);
        
        /* restituisce un vettore di stringhe contenente i nomi dei comandi disponibili
         */
        Proprieta proprieta;
};

#endif //COMANDO_H

