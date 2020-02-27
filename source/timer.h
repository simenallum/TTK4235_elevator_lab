/**
 * @file
 * @brief Functions and variables for keeping time.
*/

#ifndef timer_h
#define timer_h

#include <stdio.h>
#include "time.h"


/**
 * @brief Function that starts the timer.
*/
void timer_start();


/**
 * @brief Function that checks if the door has been opened for 3 seconds.
 * 
 * @return 1 if time is out. Otherwise it returns 0. 
*/
int timer_check_timeout();




#endif


