/**
 * @file
 * @brief State machines called from elevator
 */

#ifndef FSM_H
#define FSM_H

#define NO_ORDERS -1
#define MOTOR_DOWN -1
#define MOTOR_UP 1
#define TOP_FLOOR 3
#define BOTTOM_FLOOR 0

/**
* @brief States for the elevator.
*/
typedef enum {
	STILL = 0,
	EMERGENCY_AT_FLOOR = 1,
	EMERGENCY_BETWEEN_FLOOR = 2,
	MOVING = 3,
	DOOR_OPEN = 4
} STATES;


/**
* @brief Variable for containing current state. 
*/
extern STATES m_current_state;

/**
 * @brief Variable for containing motor direction.
 */
extern int m_motor_dir;


/**
* @brief Extern variable for containing previous floor.
*/
extern int g_prev_floor;


/**
* @brief Extern variable for containing next floor
*/
extern int g_next_floor;

/**
* @brief Array for containing up orders.
*/
extern int m_up_vec[];

/**
* @brief Array for containing down orders.
*/
extern int m_down_vec[];

/**
* @brief A state machine for initializing the elevator. 
*
* Must be called once before the while-loop main(). 
*/
void fsm_init();

/**
 * @brief A state machine for setting queue and setting the next_floor variable.
 * 
 * Sets next floor variable, and inserts the given floor @p floor, with given @p order_type into the queue. 
 *
 * @param floor to be inserted to queue.
 * @param order_type of the order to be inserted to queue.
 */
void fsm_ev_set_queue(int floor, HardwareOrder order_type);

/**
 * @brief A state machine to be called when time is out.
 * 
 */
void fsm_ev_timeout();

/**
 * @brief A state machine to be called when the elevator reaches a floor.
 * 
 * @param floor that the elevator stops at.
 */
void fsm_ev_reach_floor(int floor);

/**
 * @brief A state machine to be called when the door is being obstructed.
 */
void fsm_ev_obstruction();

/**
* @brief A state machine to be calles when the stopbutton is being pressed.
*/
void fsm_ev_stopButton_pressed();

/**
* @brief A state machine to be called for setting motor direction.
*/
void fsm_ev_request();


#endif

