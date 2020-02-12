//
//  timer.h
//  HeisXcode
//
//  Created by Sigurd Digre on 12/02/2020.
//  Copyright © 2020 Sigurd Digre. All rights reserved.
//

#ifndef timer_h
#define timer_h

#include <stdio.h>
#include "time.h"
/**
 * @file
 * @brief Functions and variables for keeping time
*/


/**
 * @brief Variable for time door opened
*/
static time_t seconds;

/**
 * @brief Function that starts time
*/
void timer_start(void);


/**
 * @brief Checks if doors have been opened for 3 seconds
*/
int check_timeout(void);




#endif


