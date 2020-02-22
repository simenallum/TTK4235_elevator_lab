#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "fsm.h"
#include "timer.h"


static void sigint_handler(int sig){
    (void)(sig);
    printf("\nTerminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}


int main(){
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);
    
    fsm_init();
    clear_all_order_lights();

    while(1){    
        if(hardware_read_stop_signal()){
            clear_all_order_lights();
            fsm_ev_stopButton_pressed();
        }

   
        for (int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; ++i){
            if(hardware_read_floor_sensor(i)){
                if (i != prev_floor){
                    hardware_command_floor_indicator_on(i);
                    prev_floor = i; // Oppdaterer prev_floor når den kjører forbi en etasje.
                }
                if(i == next_floor){
                	fsm_ev_reach_floor(i);
                }
            }
        }
 
  

        for (int floor = 0; floor < 4; floor++){
            for (int i = HARDWARE_ORDER_UP; i <= HARDWARE_ORDER_DOWN; i++){
                if(hardware_read_order(floor, i)){
                    fsm_ev_set_queue(floor, i);
                }
            }
        }

        if(check_timeout()){
            fsm_ev_timeout();
        }

        if(hardware_read_obstruction_signal()){
            fsm_ev_obstruction();
        }
        
        fsm_ev_request();

        //printf("motor_dir: %d\n", motor_dir);
        //printf("next_floor: %d\n", next_floor);
        //printf("prev_floor: %d\n", prev_floor);
        //printf("Current_state: %d\n", current_state);
    }
}