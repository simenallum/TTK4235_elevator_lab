#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "fsm.h"

void fsm_init(){
	if (hardware_read_floor_sensor(0)){
		current_state = STILL;
		prev_floor = 0;
	}

	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	while(1){
		int current_floor = 0;
		for (int i = 1; i < HARDWARE_NUMBER_OF_FLOORS; i++){
			if (hardware_read_floor_sensor(i)){
				current_floor = i;
				break;
			}
		}
		prev_floor = current_floor;
		break;
	}
	current_state = STILL;
}

void fsm_ev_set_queue(int floor, HardwareOrder order_type){

	switch(current_state){
		case MOVING:
		case DOOR_OPEN:
		case STILL:
		{
			// for setting queue.
			if (floor > prev_floor ){
				if (order_type == HARDWARE_ORDER_DOWN){
					down_vec[floor] = 1;
				}
				else{
					up_vec[floor] = 1;
				}
			}
				
			else if (floor < prev_floor){
				if (order_type == HARDWARE_ORDER_UP){
					up_vec[floor] = 1;
				}
				else{
					down_vec[floor] = 1;
				}
			}

    		// for setting the next floor variable.
			if (prev_motor_dir = 1){
				for (int i = prev_floor; i < 4; ++i){
					if (up_vec[i] == 1){
						next_floor = i;
					}
				}
			}
			else if (prev_motor_dir = -1){
				for (int i = prev_floor; i >= 0; --i){
					if (down_vec[i] == 1){
						next_floor = i;
					}
				}
			}
			else{
				for (int i = prev_floor; i < 4; ++i){
					if (up_vec[i] == 1){
						next_floor = i;
						break;
					}
				}
				for (int i = prev_floor; i >= 0; --i){
					if (down_vec[i] == 1){
						next_floor = i;
						break; 
					}
				}		
			}
			break;
		}
		case EMERGENCY_AT_FLOOR:
		case EMERGENCY_BETWEEN_FLOOR:
		break;
	}
}