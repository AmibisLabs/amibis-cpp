/**
 * @file Counter.h
 * @brief Definition of Mutexed Counter class
 */

#ifndef _MUTEXED_COUNTER_H_
#define _MUTEXED_COUNTER_H_

#include <System/Mutex.h>

/**
 * @class MutexedCounter MutexedCounter.h System/MutexedCounter.h
 * @brief Counter with access protected by mutex.
 *
 * Can be incremented, decremented and, of course, accessed.
 * It can be manipulated by multiple threads.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class MutexedCounter
{
public:
	/** @brief Constructor
	 * @param init_value [in] the initial value for the counter (0 by default).
	 */
	MutexedCounter(int init_value = 0);
	
#ifndef RAVI_INTERFACE	
	/** @name Operators */
	//@{
	/** @brief prefix increment operator*/
	int operator++(); 
	
	/** @brief postfix increment operator */
	int operator++(int);
	
	/** @brief prefix decrement operator */
	int operator--();
	
	 /** @brief postfix decrement operator */
	int operator--(int);
	
	/** @brief cast operator : return the counter value*/
	operator int() const;

	/** @brief Set the counter value to 'value' */
	int operator=(int value);
	//@}
#endif /* RAVI_INTERFACE */
private:
	ReentrantMutex mutex; /*!< the mutex to protect the access */
	int counter; /*!< the integer used to store the counter value*/
};

#endif /* _MUTEXED_COUNTER_H_ */
