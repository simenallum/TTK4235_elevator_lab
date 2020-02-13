//
//  timer.c
//  HeisXcode
//
//  Created by Sigurd Digre on 12/02/2020.
//  Copyright © 2020 Sigurd Digre. All rights reserved.
//

#include "timer.h"
#include "time.h"



void timer_start(void) {
    seconds = time(NULL);
}


int check_timeout(void) {
    if (time(NULL) - seconds >= 3) {
        return 1;
    }
    return 0;
}

