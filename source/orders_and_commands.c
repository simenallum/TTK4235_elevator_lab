#include <stdio.h>
#include <stdlib.h>
#include "orders_and_commands.h"

void set_next_floor(int prev_motor_dir, int prev_floor, int* next_floor, int up_vec[], int down_vec[]){
    // for setting the next floor variable.
			if (prev_motor_dir == 1){
				for (int i = prev_floor; i < 4; ++i){
					if (up_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
			}
			else if (prev_motor_dir == -1){
				for (int i = prev_floor; i >= 0; --i){
					if (down_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
			}
			else{
				for (int i = 0; i < 4; ++i){
					if (up_vec[i] == 1){
						*next_floor = i;
						return;
					}
				}
				for (int i = 3; i >= 0; --i){
					if (down_vec[i] == 1){
						*next_floor = i;
						return; 
					}
				}		
			}
            *next_floor = -1;
}

void clear_queue(int up_vec[], int down_vec[]){
	for (int i = 0; i < 4; i++){
		up_vec[i] = 0;
		down_vec[i] = 0;
	}
}