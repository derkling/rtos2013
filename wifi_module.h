/* 
 * File:   wifi_module.h
 * Author: r0rshark
 *
 * Created on 3 gennaio 2014, 17.29
 */

#ifndef WIFI_MODULE_H
#define	WIFI_MODULE_H


void invia(char *payload);
void *wifi_start(void *arg);
void *wifi_receive(void *arg);
void *wifi_transmit(void *arg);
void ricevi(char *payload);

#endif	/* WIFI_MODULE_H */

