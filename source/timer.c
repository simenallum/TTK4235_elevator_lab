#include "timer.h"
#include "time.h"

static time_t m_seconds = 0;

void timer_start() {
    m_seconds = time(NULL);
}


int timer_check_timeout() {
    if (time(NULL) - m_seconds >= 3) {
        return 1;
    }
    return 0;
}