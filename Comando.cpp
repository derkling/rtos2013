/* 
 * File:   Comando.cpp
 * Author: Liscio-Cazzetta
 * 
 * la classe Comando.cpp gestisce i comandi presenti nel vector comandi, quindi
 * in base all'input dell'utente restituisce un valore di una proprieta passata
 * per input oppure valori booleani per il controllo di correttezza di input
 */

#include <cstdio>
#include "miosix.h"
#include <unistd.h>
#include <cstring>
#include <map>
#include "Proprieta.h"
#include "Comando.h"

using namespace std;

Comando::Comando(){
    strncpy(comandi[0].comando, "read", 10);
    strncpy(comandi[1].comando, "update", 10);
    strncpy(comandi[2].comando, "subscibe", 10);
    strncpy(comandi[3].comando, "check", 10);
    strncpy(comandi[4].comando, "notify", 10);
    strncpy(comandi[5].comando, "deposit", 10);
}


bool Comando::esisteComando(const char *stringa){
    for(int i=0; comandi[i].comando!=NULL; i++)
        if (strcmp(comandi[i].comando,stringa)==0)
            return true;
    return false;
}

int Comando::read(const char *nomeProprieta){
        return proprieta.getValoreProprieta(nomeProprieta);
}

void Comando::update(const char *nomeProprieta, int valore){
}

void Comando::store(const char *nomeProprieta, int valore){
}

void Comando::subscribe(const char *nomeModulo, const char *nomeProprieta){
}

bool Comando::check(const char *nomeModulo){
	bool controllo=false;
	return controllo;
}

void Comando::notify(const char *nomeProprieta, int valore){
}
