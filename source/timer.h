//
//  timer.h
//  HeisXcode
//
//  Created by Sigurd Digre on 12/02/2020.
//  Copyright © 2020 Sigurd Digre. All rights reserved.
//

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
int check_timeout();




#endif


