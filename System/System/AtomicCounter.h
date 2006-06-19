/**
 * @file AtomicCounter.h
 * @brief Definition of AtomicCounter class
 */

#ifndef _ATOMIC_COUNTER_H_
#define _ATOMIC_COUNTER_H_

#include <System/Mutex.h>

/**
 * @class AtomicCounter AtomicCounter.h System/AtomicCounter.h
 * @brief Counter with access protected by mutex.
 *
 * Can be incremented, decremented and, of course, accessed.
 * It can be manipulated by multiple threads.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class AtomicCounter
{
public:
	/** @brief Constructor
	 * @param init_value [in] the initial value for the counter (0 by default).
	 */
	AtomicCounter(int init_value = 0);
	
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

#endif /* _ATOMIC_COUNTER_H_ */
