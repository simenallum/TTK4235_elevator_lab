/**
 * @file
 * @brief State machines used in elevator.
 */

#ifndef FSM_H
#define FSM_H

#define NO_ORDERS -1
#define MOTOR_DOWN -1
#define MOTOR_UP 1
#define TOP_FLOOR 3
#define BOTTOM_FLOOR 0

/**
* @brief States for the elevator
*/

typedef enum {
	STILL = 0,
	EMERGENCY_AT_FLOOR = 1,
	EMERGENCY_BETWEEN_FLOOR = 2,
	MOVING = 3,
	DOOR_OPEN = 4
} STATES;


/**
* @brief static variable for containing current state. 
*/
extern STATES current_state;

/**
 * @brief Containing motor direction.
 */
extern int motor_dir;


/**
* @brief Previous floor.
*/
extern int prev_floor;


/**
* @brief Next floor
*/
extern int next_floor;

/**
* @brief An array for containing up orders.
*/
extern int up_vec[];

/**
* @brief Next floorAn array for containing down orders.
*/
extern int down_vec[];

/**
* @brief A state machine for initializing the elevator.
*/
void fsm_init();

/**
 * @brief Sets next floor variable, and inserts the given floor @p floor into the queue. 
 *
 * @param floor too be inserted to queue.
 *
 * @return 0 if all good. 1 if there was an error.
 */
void fsm_ev_set_queue(int floor, HardwareOrder order_type);

/**
 * @brief a state machine to be called when timer is up.
 */
void fsm_ev_timeout();

/**
 * @brief a state machine for the event that the elevator reaches a floor.
 * 
 * @param floor that the elevator stops at.
 */
void fsm_ev_reach_floor(int floor);

/**
 * @brief a state machine for the event that the door is being obstructed.
 */
void fsm_ev_obstruction();

/**
* @brief a state machine for the event stop button is being pressed.
*/
void fsm_ev_stopButton_pressed();

/**
* @brief a state machine for handling requests.
*/
void fsm_ev_request();


#endif

