#ifndef PARAM_H
#define PARAM_H

#include <Arduino.h>

//=====================================
//PINS
#define SPK_1 25
#define SPK_2 26
#define SWPIN 32
#define GAS 34

//=====================================
//MACROS / PARAMETERS / AUX FUNCTIONS
#define rep(i, n) for(int i=0; i<n; i++)
#define MAX_ARDUINO_TIME 3294967295

unsigned long get_time(){
    return (millis()%MAX_ARDUINO_TIME);
}


//=====================================


#endif