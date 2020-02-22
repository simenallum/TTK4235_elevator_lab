/**
 * @file
 * @brief A library for containing queue functions.
 */ 

#define NO_ORDERS -1
#define MOTOR_DOWN -1
#define MOTOR_UP 1
#define TOP_FLOOR 3
#define BOTTOM_FLOOR 0

/**
 * @brief Sets the @p next_floor variable.
 * 
 * @param prev_motor_dir previous motor direction.
 * @param prev_floor previous floor. 
 * @param next_floor to be set.
 * @param up_vec is the array containing upp orders.
 * @param down_vec is the array containing down orders.
 */
void set_next_floor( 
                    int prev_motor_dir, 
                    int prev_floor, 
                    int* next_floor, 
                    int up_vec[], 
                    int down_vec[]
                    );

/**
 * @brief a function for clearing the queue in case of an emergency.
 *
 * @param up_vec is the vector containing upp orders.
 * @param down_vec is the vector for containing down orders.
 */
void clear_queue(int up_vec[], int down_vec[]);