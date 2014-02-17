/**
 * File:   wifi_module.h
 *  \brief     This is the header file for the file wifi_module.cpp.
 *  \author    Andrea Corna
 *  \author    Lorenzo Fontana
 *  \version   1.0
 *  \date      03/01/2014
 */

#ifndef WIFI_MODULE_H
#define	WIFI_MODULE_H

/**
 * Method in order to send data with the wireless module. Max lenght of payload should be
 * 32 characters.
 * @param payload - payload to be sent
 */
void send(char *payload);

/**
 * Function to be executed by a thread in order to receive data from other boards.
 * @param arg - something to give to function 
 * @return 
 */
void *wifi_receive(void *arg);

/**
 * Function to be  by a thread in order to send data to other boards.
 * @param arg - something to give to function 
 * @return 
 */
void *wifi_transmit(void *arg);

/**
 * Function in order to receive data from module's buffer. 
 * @param payload - char array with size equals to 960
 */
void receive(char *payload);

/**
 * Function in order to setup wireless module
 */
void init();
#endif	/* WIFI_MODULE_H */

