/* 
 * File:   nRF24L01P.h
 * Author: r0rshark
 *
 * Created on 3 gennaio 2014, 17.22
 */
#include "spi_driver.h"

#ifndef NRF24L01P_H
#define	NRF24L01P_H


class nRF24L01P {
public:
    nRF24L01P();
    nRF24L01P(const nRF24L01P& orig);
    virtual ~nRF24L01P();    
    void power_up();
    void power_down();
    void set_transmit_mode();
    void set_receive_mode();
    int transmit(int count, char* data);
    int receive(int pipe,char *data,int count);
    void test_receive(); 
    void test_transmit();
private:
    void CE_enable();
    void CE_disable();
    void set_register(int addr_registro,int data_registro);
    int get_register(int registro);
    int get_register_status();
    void CE_restore(int old_ce);
    bool packet_in_pipe(int pipe);
    spi_driver *spi;
    void setup_Gpio();
    void set_frequency(int frequency);
    void set_power_output(int power);
    void set_air_data_rate(int rate);
    int mode;

};

#endif	/* NRF24L01P_H */

