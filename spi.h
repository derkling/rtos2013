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
    void send(unsigned char dataByte);
    void send(int data);
    unsigned char receive();
private:

};



#endif	/* SPI_H */

