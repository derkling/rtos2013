/* 
 * File:   Proprieta.cpp
 * Author: Liscio-Cazzetta
 * 
 * la classe Proprieta gestisce le proprietà memorizzate nella board
 * permettendo, tramite terminale, una facile localizzazione
 * e quindi, utilizzando la classe Comando.cpp, una facile gestione
 * per eventuali read, update eccetera.
 */

#ifndef PROPRIETA_H
#define	PROPRIETA_H

#include <cstdio>
#include "miosix.h"
#include <unistd.h>
#include <cstring>
#include <map>

class Proprieta {
public:
    
    //comparatore per ordinare la lista
    struct char_cmp { 
        bool operator () (const char *a,const char *b) const {
        return strcmp(a,b)<0;
        } 
    };

    //char *=tipo della chiave primaria(stringa) int=tipo del valore assunto,
    //char_cmp=comparatore che ordina gli elementi della mappa
    typedef std::map<const char *, int, char_cmp> Map;

    Map proprieta;
    
    /* il costruttore inizializza la lista delle proprieta effettivamente
     * utlizzate dalla board e dagli utenti
     */
    Proprieta();
    
    /*restituisce true se il nome del parametro equivale ad un nome di proprieta
     * memorizzato nella Map
     */
    bool esisteProprieta(const char* stringa);
    
    /* restituisce il valore della proprietà corrispondente al parametro passato
     */
    int getValoreProprieta(const char* nome);
private:

};

#endif	/* PROPRIETA_H */

