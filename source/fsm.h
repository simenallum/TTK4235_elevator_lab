/**
 * @file
 * @brief State machines used in elevator.
 */


/**
* @brief States for the elevator
*/
typedef enum {
	STILL = 0,
	EMERGENCY_AT_FLOOR = 1,
	EMERGENCY_BETWEEN_FLOOR = 2,
	MOVING_UP = 3,
	MOVING_DOWN = 4,
	DOOR_OPEN = 5
} STATES;



/**
* @brief static variable for containing current state. 
*/
static STATES current_state;


/**
* @brief Previous floor, signed
*/
static int pre_real_floor;


/**
* @brief Next floor
*/
static int next_floor;

/**
* @brief variable for containing motor direction.
*/
static int motor_dir;

/**
* @brief A state machine for initializing the elevator.
*/
void fsm_init();

