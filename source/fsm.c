#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "fsm.h"

void fsm_init(){
	if (hardware_read_floor_sensor(0)){
		current_state = STILL;
		pre_real_floor = 0;
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
		pre_real_floor = current_floor;
		break;
	}
	current_state = STILL;
}