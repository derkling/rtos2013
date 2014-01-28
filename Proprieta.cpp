/* 
 * File:   Proprieta.cpp
 * Author: Liscio-Cazzetta
 * 
 * la classe Proprieta gestisce le proprietà memorizzate nella board
 * permettendo, tramite terminale, una facile localizzazione
 * e quindi, utilizzando la classe Comando.cpp, una facile gestione
 * per eventuali read, update eccetera.
 */

#include <cstdio>
#include "miosix.h"
#include <unistd.h>
#include <cstring>
#include <map>
#include "Proprieta.h"

using namespace std;
using namespace miosix;


Proprieta::Proprieta() {
    proprieta.insert(Map::value_type("passi",50));
    proprieta.insert(Map::value_type("peso",90));
    proprieta.insert(Map::value_type("eta",50));
    proprieta.insert(Map::value_type("frequenza",90));
}

bool Proprieta::esisteProprieta(const char* stringa){
    if(proprieta.find(stringa)!=proprieta.end())
        return true;
    return false;
}
int Proprieta::getValoreProprieta(const char *nome) {
    return proprieta[nome];
}

