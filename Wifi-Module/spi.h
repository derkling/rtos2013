/* 
 * File:   spi.h
 * Author: degrigis
 *
 * Created on January 9, 2014, 4:57 PM
 */

#ifndef SPI_H
#define	SPI_H

class spi {
public:
    spi();
    spi(const spi& orig);
    virtual ~spi();
    void send(int data);
    int receive();
private:

};



#endif	/* SPI_H */

