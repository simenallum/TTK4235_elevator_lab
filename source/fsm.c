#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "fsm.h"
#include "orders_and_commands.h"
#include "timer.h"

	STATES current_state;
	int motor_dir;
	int prev_floor;
	int next_floor;
	int up_vec[HARDWARE_NUMBER_OF_FLOORS] = {0,0,0,0};
	int down_vec[HARDWARE_NUMBER_OF_FLOORS] = {0,0,0,0};
	int motor_dir;

void fsm_init(){


	if (hardware_read_floor_sensor(BOTTOM_FLOOR)){
		current_state = STILL;
		prev_floor = BOTTOM_FLOOR;
		next_floor = NO_ORDERS;
		motor_dir = MOTOR_UP;
		hardware_command_floor_indicator_on(BOTTOM_FLOOR);
        return;
	}
    else if (hardware_read_floor_sensor(TOP_FLOOR)){
        current_state = STILL;
        prev_floor = TOP_FLOOR;
        next_floor = NO_ORDERS;
		motor_dir = MOTOR_UP;
        hardware_command_floor_indicator_on(TOP_FLOOR);
        return;	
    }
    
	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	int current_floor = 0;
	while(1){
		for (int i = BOTTOM_FLOOR+1; i < TOP_FLOOR; i++){
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
	next_floor = NO_ORDERS;
	motor_dir = MOTOR_UP;
}

void fsm_ev_set_queue(int floor, HardwareOrder order_type){

	switch(current_state){
		case DOOR_OPEN:
		{
			if(prev_floor == floor){return;}
		}
		case MOVING:
		case STILL:
		{
			hardware_command_order_light(floor, order_type, 1);
			// for setting queue.
			if (floor > prev_floor ){
				if (order_type == HARDWARE_ORDER_DOWN){
					if ((floor == TOP_FLOOR) && (order_type == HARDWARE_ORDER_DOWN)){
						up_vec[TOP_FLOOR] = 1;
					}
					else{
						down_vec[floor] = 1;
					}
				}
				else{
					up_vec[floor] = 1;
				}
			}
				
			else if (floor < prev_floor){
				if (order_type == HARDWARE_ORDER_UP){
					if ((floor == BOTTOM_FLOOR) && (order_type == HARDWARE_ORDER_UP)){
						down_vec[BOTTOM_FLOOR] = 1;
					}
					else{
						up_vec[floor] = 1;
					}
					
				}
				else{
					down_vec[floor] = 1;
				}
			}

			else if(floor == prev_floor){
				if(order_type == HARDWARE_ORDER_UP){
					if ((floor == BOTTOM_FLOOR) && (order_type == HARDWARE_ORDER_UP)){
						down_vec[BOTTOM_FLOOR] = 1;
					}
					else{
						up_vec[floor] = 1;
					}
				}
				else if(order_type == HARDWARE_ORDER_DOWN){
					if ((floor == TOP_FLOOR) && (order_type == HARDWARE_ORDER_DOWN)){
						up_vec[TOP_FLOOR] = 1;
					}
					else{
						down_vec[floor] = 1;
					}
				}
				else if(order_type == HARDWARE_ORDER_INSIDE){
					if(motor_dir == MOTOR_UP){
						down_vec[floor] = 1;
					}
					else if(motor_dir == MOTOR_DOWN){
						up_vec[floor] = 1;
					}
				}
			}

    		oc_set_next_floor(motor_dir, prev_floor, &next_floor, up_vec, down_vec);
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
			hardware_command_door_open(0); // closes door.
			current_state = STILL;
			break; //Lagt til en endring her. Manglet brake.
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
			hardware_command_door_open(1); //Opens door (lights door-light)
			timer_start();
			up_vec[floor] = 0; //clearing queues. 
			down_vec[floor] = 0;
			for (int i = HARDWARE_ORDER_UP; i <= HARDWARE_ORDER_DOWN; i++){
				hardware_command_order_light(floor, i, 0); //turn off order lights.
			}
			oc_set_next_floor(motor_dir, prev_floor, &next_floor, up_vec, down_vec); //sets next floor
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
	oc_clear_queue(&up_vec[0], &down_vec[0]);
	next_floor = NO_ORDERS;

	while(hardware_read_stop_signal()){
		switch(current_state){
			case STILL:
			{
				for(int i = BOTTOM_FLOOR; i <= TOP_FLOOR; i++){
					if(hardware_read_floor_sensor(i)){
						current_state = EMERGENCY_AT_FLOOR;
						timer_start();
						hardware_command_door_open(1);
						break;
					}

				}
				if(current_state == EMERGENCY_AT_FLOOR){break;}
				current_state = EMERGENCY_BETWEEN_FLOOR; 
				break;

			}
			case EMERGENCY_AT_FLOOR:
			case DOOR_OPEN:
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
					if (next_floor == NO_ORDERS){
						hardware_command_movement(HARDWARE_MOVEMENT_STOP);
						current_state = STILL;
						return;
					}

					if(next_floor > prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_UP);
						motor_dir = MOTOR_UP;
					}
					else if(next_floor < prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
						motor_dir = MOTOR_DOWN;
					}
					else if(next_floor == prev_floor){
						if (motor_dir == MOTOR_UP){
							hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
							motor_dir = MOTOR_UP;
						}
						else if (motor_dir == MOTOR_DOWN){
							hardware_command_movement(HARDWARE_MOVEMENT_UP);
							motor_dir = MOTOR_UP;
						}
					}
					
					current_state = MOVING;
					return;
				}						
			case DOOR_OPEN:
			case MOVING:
			case EMERGENCY_BETWEEN_FLOOR:
			case EMERGENCY_AT_FLOOR:
				break;
	}
}