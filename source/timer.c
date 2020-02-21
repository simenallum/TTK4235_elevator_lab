//
//  timer.c
//  HeisXcode
//
//  Created by Sigurd Digre on 12/02/2020.
//  Copyright © 2020 Sigurd Digre. All rights reserved.
//

#include "timer.h"
#include "time.h"

static time_t seconds = 0;

void timer_start() {
    seconds = time(NULL);
    printf("Timer started\n");
}


int check_timeout() {
    if (time(NULL) - seconds >= 3) {
        return 1;
    }
    return 0;
}

