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
	int up_vec[] = {0,0,0,0};
	int down_vec[] = {0,0,0,0};
	int motor_dir;

void fsm_init(){


	if (hardware_read_floor_sensor(0)){
		current_state = STILL;
		prev_floor = 0;
		next_floor = -1;
		motor_dir = 1;
		hardware_command_floor_indicator_on(0);
        return;
	}
    else if (hardware_read_floor_sensor(3)){
        current_state = STILL;
        prev_floor = 3;
        next_floor = -1;
		motor_dir = 1;
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
	motor_dir = 1;
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
					// Her prioriteres en ned bestilling i 4, og den legges i opp-vektor.
					if ((floor == 3) && (order_type == HARDWARE_ORDER_DOWN)){
						hardware_command_order_light(3, HARDWARE_ORDER_DOWN, 1);
						up_vec[3] = 1;
					}
					//
					else{
						down_vec[floor] = 1;
						hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 1);
					}
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
					// Her prioriteres en opp bestilling i 1, og legges i ned vektor.
					if ((floor == 0) && (order_type == HARDWARE_ORDER_UP)){
						down_vec[0] = 1;
						hardware_command_order_light(0, HARDWARE_ORDER_UP, 1);
					}
					//
					else{
						up_vec[floor] = 1;
						hardware_command_order_light(floor, HARDWARE_ORDER_UP, 1);
					}
					
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

			else if(floor == prev_floor){
				if(order_type == HARDWARE_ORDER_UP){
					if ((floor == 0) && (order_type == HARDWARE_ORDER_UP)){
						down_vec[0] = 1;
						hardware_command_order_light(0, HARDWARE_ORDER_UP, 1);
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
				else if(order_type == HARDWARE_ORDER_DOWN){
					if ((floor == 3) && (order_type == HARDWARE_ORDER_DOWN)){
						hardware_command_order_light(3, HARDWARE_ORDER_DOWN, 1);
						up_vec[3] = 1;
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
				else if(order_type == HARDWARE_ORDER_INSIDE){
					if(motor_dir == 1){
						down_vec[floor] = 1;
						hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 1);
					}
					else if(motor_dir == -1){
						up_vec[floor] = 1;
						hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 1);
					}
				}
			}

			printf(">Up vec: [");
			for (int i = 0; i < 4; i++){
				printf("%d", up_vec[i]); 
				printf(" ");
			}
			printf("]\n");
			printf("Down VEc: [");
			for (int i = 0; i < 4; i++){
				printf("%d", down_vec[i]); 
				printf(" ");
			}
			printf("]\n");

    		set_next_floor(motor_dir, prev_floor, &next_floor, up_vec, down_vec);
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
			return; //Lagt til en endring her. Manglet return / brake.
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
		case STILL: // Den skal kunne entre reach floor fra still også. Da den kan stå i en etasje med døren lukkket.
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
			set_next_floor(motor_dir, prev_floor, &next_floor, up_vec, down_vec); //sets next floor
			current_state = DOOR_OPEN;
			break;

		}
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
						// motor_dir = 0; // denne vil være dumt å ha med da den setter motor_dir = 0 rett etter et stop.
						return;
					}

					if(next_floor > prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_UP);
						motor_dir = 1;
					}
					else if(next_floor < prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
						motor_dir = -1;
					}
					else if(next_floor == prev_floor){ //denne vil bare kalles etter en eventuell stop mellom to etasjer.
						if (motor_dir == 1){
							hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
							motor_dir = -1;
						}
						else if (motor_dir == -1){
							hardware_command_movement(HARDWARE_MOVEMENT_UP);
							motor_dir = 1;
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