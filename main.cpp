/**
 * File:   lis302dl.cpp
 *  \brief     main class. It only starts the program
 *  \author    Omar Scotti
 *  \author    Diego Rondelli
 *  \version   1.0
 *  \date      14/01/2014
 */

#include <cstdlib>
#include <cstdio>
#include "pedometer.h"

using namespace std;

Pedometer pedometer;

/*
 * Main method, starts the pedometer
 */
int main(int argc, char** argv) {
    pedometer.start();
    return 0;
}

