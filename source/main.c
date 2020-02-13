#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "fsm.h"

static void clear_all_order_lights(){
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        for(int i = 0; i < 3; i++){
            HardwareOrder type = order_types[i];
            hardware_command_order_light(f, type, 0);
        }
    }
}


int main(){
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    fsm_init();

    while(1){
        if(hardware_read_stop_signal()){
            fsm_ev_stopButton_pressed();
        }

        for (int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; ++i){
            if(hardware_read_floor_sensor(i)){
                if (i != prev_floor){
                    hardware_command_floor_indicator_on(i);
                }
                fsm_ev_reach_floor(i);
            }
        }

        for (int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++){
            for (int i = HARDWARE_ORDER_UP; i <= HARDWARE_ORDER_DOWN; i++){
                if ((floor == 0) && (i == HARDWARE_ORDER_DOWN)){
                    continue;
                }
                if ((floor == 3) && (i == HARDWARE_ORDER_UP)){
                    continue;
                }
                fsm_ev_set_queue(floor, i);
            }
        }

        if(check_timeout()){
            fsm_ev_timeout();
        }

        if(hardware_read_obstruction_signal()){
            fsm_ev_obstruction();
        }
        
        fsm_ev_request();
    }

    /**
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    hardware_command_movement(HARDWARE_MOVEMENT_UP);

    while(1){
        if(hardware_read_stop_signal()){
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;
        }

        if(hardware_read_floor_sensor(0)){
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
        }
        if(hardware_read_floor_sensor(HARDWARE_NUMBER_OF_FLOORS - 1)){
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        }
    }
    */
}
