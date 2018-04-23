/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Contains the definition for a type that should be returned by most functions.
 * This status code can be used to determine whether the system is in an error
 * state.
 */
#ifndef STATUS_H
#define STATUS_H

/**
 * Status code that most functions should return. Proper design is to assert
 * that every function returns 'Success'.
 */
typedef enum status {
	Fail = 0,
	Success = 1
} status_t;

#endif
