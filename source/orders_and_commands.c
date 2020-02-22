#include <stdio.h>
#include <stdlib.h>
#include "orders_and_commands.h"


void set_next_floor(int motor_dir, int prev_floor, int* next_floor, int up_vec[], int down_vec[]){
    // for setting the next floor variable.
			if(prev_floor == BOTTOM_FLOOR){
				motor_dir = MOTOR_UP;
			}
			else if(prev_floor == TOP_FLOOR){
				motor_dir = MOTOR_DOWN;
			}
			
			if (motor_dir == MOTOR_UP){
				for (int i = prev_floor+1; i <= TOP_FLOOR; ++i){
					if (up_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
				for (int i = TOP_FLOOR; i >= BOTTOM_FLOOR; --i){
					if (down_vec[i] == 1){
						*next_floor = i;
						return; 
					}
				}
				for (int i = BOTTOM_FLOOR; i <= prev_floor; ++i){
					if (up_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
			}
			else if (motor_dir == MOTOR_DOWN){
				for (int i = prev_floor-1; i >= BOTTOM_FLOOR; --i){
					if (down_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
				for (int i = BOTTOM_FLOOR; i <= TOP_FLOOR; ++i){
					if (up_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
				for (int i = TOP_FLOOR; i >= prev_floor; --i){
					if (down_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
			}
            *next_floor = NO_ORDERS;
}

void clear_queue(int up_vec[], int down_vec[]){
	for (int i = BOTTOM_FLOOR; i < TOP_FLOOR; i++){
		up_vec[i] = 0;
		down_vec[i] = 0;
	}
}