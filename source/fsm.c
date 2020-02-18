#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "fsm.h"
#include "orders_and_commands.h"
#include "timer.h"

	STATES current_state;
	int prev_motor_dir;
	int prev_floor;
	int next_floor;
	int up_vec[] = {0,0,0,0};
	int down_vec[] = {0,0,0,0};
	int motor_dir;

void fsm_init(){


	if (hardware_read_floor_sensor(0)){
		current_state = STILL;
		prev_floor = 0;
		next_floor = -1;
		hardware_command_floor_indicator_on(0);
        return;
	}
    else if (hardware_read_floor_sensor(3)){
        current_state = STILL;
        prev_floor = 3;
        next_floor = -1;
        hardware_command_floor_indicator_on(3);
        return;	
    }
    
	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	int current_floor = 0;
	while(1){
		for (int i = 1; i < 4; i++){
			if (hardware_read_floor_sensor(i)){
				hardware_command_movement(HARDWARE_MOVEMENT_STOP);
				current_floor = i;
				hardware_command_floor_indicator_on(i);
				break;
			}
		}
		prev_floor = current_floor;
		if (current_floor != 0){
			break;
		}	
	}
	current_state = STILL;
	next_floor = -1;
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
					hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 1);
				}
				else{
					up_vec[floor] = 1;
					if (order_type == HARDWARE_ORDER_UP){
						hardware_command_order_light(floor, HARDWARE_ORDER_UP, 1);
					}
					else if(order_type == HARDWARE_ORDER_INSIDE){
						hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 1);
					}
				}
			}
				
			else if (floor < prev_floor){
				if (order_type == HARDWARE_ORDER_UP){
					up_vec[floor] = 1;
					hardware_command_order_light(floor, HARDWARE_ORDER_UP, 1);
					
				}
				else{
					down_vec[floor] = 1;
					if (order_type == HARDWARE_ORDER_DOWN){
						hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 1);
					}
					else if(order_type == HARDWARE_ORDER_INSIDE){
						hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 1);
					}
				}
			}
    		set_next_floor(prev_motor_dir, prev_floor, &next_floor, up_vec, down_vec);
			break;
		}
		case EMERGENCY_AT_FLOOR:
		case EMERGENCY_BETWEEN_FLOOR:
		break;
	}
}


void fsm_ev_timeout(){
	switch(current_state){
		case DOOR_OPEN:
		{
			printf("timer time_out\n");
			hardware_command_door_open(0); // closes door.
			current_state = STILL;
		}
		case MOVING:
		case STILL:
		case EMERGENCY_BETWEEN_FLOOR:
		case EMERGENCY_AT_FLOOR:
		break;
	}
}

void fsm_ev_reach_floor(int floor){
	switch(current_state){
		case MOVING:
		{
			hardware_command_movement(HARDWARE_MOVEMENT_STOP); //stoping the elevator
			prev_floor = floor;
			hardware_command_door_open(1); //Opens door (lights door-light)
			timer_start();
			up_vec[floor] = 0; //clearing queues. 
			down_vec[floor] = 0;
			for (int i = HARDWARE_ORDER_UP; i <= HARDWARE_ORDER_DOWN; i++){
				hardware_command_order_light(floor, i, 0); //turn off order lights.
			}
			set_next_floor(prev_motor_dir, prev_floor, &next_floor, up_vec, down_vec); //sets next floor
			current_state = DOOR_OPEN;
			break;

		}
		case STILL:
		case EMERGENCY_BETWEEN_FLOOR:
		case EMERGENCY_AT_FLOOR:
		case DOOR_OPEN:
		break;
	}
}


void fsm_ev_obstruction(){
	switch(current_state){
		case DOOR_OPEN:
		{
			timer_start();
			break;
		}
		case MOVING:
		case STILL:
		case EMERGENCY_BETWEEN_FLOOR:
		case EMERGENCY_AT_FLOOR:
		break;
	}
}


void fsm_ev_stopButton_pressed(){
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);
	hardware_command_stop_light(1);
	clear_queue(up_vec, down_vec);
	next_floor = -1;

	while(hardware_read_stop_signal()){
		switch(current_state){
			case EMERGENCY_AT_FLOOR:
			case DOOR_OPEN:
			case STILL:
			{
				current_state = EMERGENCY_AT_FLOOR;
				timer_start();
				break;
			}
			case EMERGENCY_BETWEEN_FLOOR:
			case MOVING:
			{
				current_state = EMERGENCY_BETWEEN_FLOOR;
				break;
			}
		}
	}
	hardware_command_stop_light(0);
	if (current_state == EMERGENCY_AT_FLOOR){
		current_state = DOOR_OPEN;
	}
	else{
		current_state = STILL;
	}
}

void fsm_ev_request(){
	switch(current_state){
			case STILL:
				{
					if (next_floor == -1){
						hardware_command_movement(HARDWARE_MOVEMENT_STOP);
						prev_motor_dir = 0;
						break;
					}

					if(next_floor > prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_UP);
						prev_motor_dir = 1;
					}
					else if(next_floor < prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
						prev_motor_dir = -1;
					}
					
					current_state = MOVING;
					break;
				}						
			case DOOR_OPEN:
			case MOVING:
			case EMERGENCY_BETWEEN_FLOOR:
			case EMERGENCY_AT_FLOOR:
				break;
	}
}




