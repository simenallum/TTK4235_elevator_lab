#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "fsm.h"
#include "orders_and_commands.h"
#include "timer.h"

	int g_prev_floor;
	int g_next_floor;

	STATES m_current_state;
	int m_motor_dir;
	int m_up_vec[HARDWARE_NUMBER_OF_FLOORS] = {0,0,0,0};
	int m_down_vec[HARDWARE_NUMBER_OF_FLOORS] = {0,0,0,0};

void fsm_init(){


	if (hardware_read_floor_sensor(BOTTOM_FLOOR)){
		m_current_state = STILL;
		g_prev_floor = BOTTOM_FLOOR;
		g_next_floor = NO_ORDERS;
		m_motor_dir = MOTOR_UP;
		hardware_command_floor_indicator_on(BOTTOM_FLOOR);
        return;
	}
    else if (hardware_read_floor_sensor(TOP_FLOOR)){
        m_current_state = STILL;
        g_prev_floor = TOP_FLOOR;
        g_next_floor = NO_ORDERS;
		m_motor_dir = MOTOR_UP;
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
		g_prev_floor = current_floor;
		if (current_floor != 0){
			break;
		}	
	}
	m_current_state = STILL;
	g_next_floor = NO_ORDERS;
	m_motor_dir = MOTOR_UP;
}

void fsm_ev_set_queue(int floor, HardwareOrder order_type){

	switch(m_current_state){
		case DOOR_OPEN:
		{
			if(g_prev_floor == floor){return;}
		}
		case MOVING:
		case STILL:
		{
			hardware_command_order_light(floor, order_type, 1);
			// for setting queue.
			if (floor > g_prev_floor ){
				if (order_type == HARDWARE_ORDER_DOWN){
					if ((floor == TOP_FLOOR) && (order_type == HARDWARE_ORDER_DOWN)){
						m_up_vec[TOP_FLOOR] = 1;
					}
					else{
						m_down_vec[floor] = 1;
					}
				}
				else{
					m_up_vec[floor] = 1;
				}
			}
				
			else if (floor < g_prev_floor){
				if (order_type == HARDWARE_ORDER_UP){
					if ((floor == BOTTOM_FLOOR) && (order_type == HARDWARE_ORDER_UP)){
						m_down_vec[BOTTOM_FLOOR] = 1;
					}
					else{
						m_up_vec[floor] = 1;
					}
					
				}
				else{
					m_down_vec[floor] = 1;
				}
			}

			else if(floor == g_prev_floor){
				if(order_type == HARDWARE_ORDER_UP){
					if ((floor == BOTTOM_FLOOR) && (order_type == HARDWARE_ORDER_UP)){
						m_down_vec[BOTTOM_FLOOR] = 1;
					}
					else{
						m_up_vec[floor] = 1;
					}
				}
				else if(order_type == HARDWARE_ORDER_DOWN){
					if ((floor == TOP_FLOOR) && (order_type == HARDWARE_ORDER_DOWN)){
						m_up_vec[TOP_FLOOR] = 1;
					}
					else{
						m_down_vec[floor] = 1;
					}
				}
				else if(order_type == HARDWARE_ORDER_INSIDE){
					if(m_motor_dir == MOTOR_UP){
						m_down_vec[floor] = 1;
					}
					else if(m_motor_dir == MOTOR_DOWN){
						m_up_vec[floor] = 1;
					}
				}
			}

    		oc_set_next_floor(m_motor_dir, g_prev_floor, &g_next_floor, m_up_vec, m_down_vec);
			break;
		}
		case EMERGENCY_AT_FLOOR:
		case EMERGENCY_BETWEEN_FLOOR:
			break;
	}
}


void fsm_ev_timeout(){
	switch(m_current_state){
		case DOOR_OPEN:
		{
			hardware_command_door_open(0);
			m_current_state = STILL;
			break;
		}
		case MOVING:
		case STILL:
		case EMERGENCY_BETWEEN_FLOOR:
		case EMERGENCY_AT_FLOOR:
			break;
	}
}

void fsm_ev_reach_floor(int floor){
	switch(m_current_state){
		case MOVING:
		{
			hardware_command_movement(HARDWARE_MOVEMENT_STOP);
			hardware_command_door_open(1);
			timer_start();
			m_up_vec[floor] = 0; 
			m_down_vec[floor] = 0;
			for (int i = HARDWARE_ORDER_UP; i <= HARDWARE_ORDER_DOWN; i++){
				hardware_command_order_light(floor, i, 0);
			}
			oc_set_next_floor(m_motor_dir, g_prev_floor, &g_next_floor, m_up_vec, m_down_vec);
			m_current_state = DOOR_OPEN;
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
	switch(m_current_state){
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
	oc_clear_queue(&m_up_vec[0], &m_down_vec[0]);
	g_next_floor = NO_ORDERS;

	while(hardware_read_stop_signal()){
		switch(m_current_state){
			case STILL:
			{
				for(int i = BOTTOM_FLOOR; i <= TOP_FLOOR; i++){
					if(hardware_read_floor_sensor(i)){
						m_current_state = EMERGENCY_AT_FLOOR;
						timer_start();
						hardware_command_door_open(1);
						break;
					}

				}
				if(m_current_state == EMERGENCY_AT_FLOOR){break;}
				m_current_state = EMERGENCY_BETWEEN_FLOOR; 
				break;

			}
			case EMERGENCY_AT_FLOOR:
			case DOOR_OPEN:
			{
				m_current_state = EMERGENCY_AT_FLOOR;
				timer_start();
				break;
			}
			case EMERGENCY_BETWEEN_FLOOR:
			case MOVING:
			{
				m_current_state = EMERGENCY_BETWEEN_FLOOR;
				break;
			}
		}
	}
	hardware_command_stop_light(0);
	if (m_current_state == EMERGENCY_AT_FLOOR){
		m_current_state = DOOR_OPEN;
	}
	else{
		m_current_state = STILL;
	}
}

void fsm_ev_request(){
	switch(m_current_state){
			case STILL:
				{
					if (g_next_floor == NO_ORDERS){
						hardware_command_movement(HARDWARE_MOVEMENT_STOP);
						m_current_state = STILL;
						return;
					}

					if(g_next_floor > g_prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_UP);
						m_motor_dir = MOTOR_UP;
					}
					else if(g_next_floor < g_prev_floor){
						hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
						m_motor_dir = MOTOR_DOWN;
					}
					else if(g_next_floor == g_prev_floor){
						if (m_motor_dir == MOTOR_UP){
							hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
							m_motor_dir = MOTOR_UP;
						}
						else if (m_motor_dir == MOTOR_DOWN){
							hardware_command_movement(HARDWARE_MOVEMENT_UP);
							m_motor_dir = MOTOR_UP;
						}
					}
					
					m_current_state = MOVING;
					return;
				}						
			case DOOR_OPEN:
			case MOVING:
			case EMERGENCY_BETWEEN_FLOOR:
			case EMERGENCY_AT_FLOOR:
				break;
	}
}