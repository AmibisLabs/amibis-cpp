/**
 * @file System/AtomicReentrantCounter.h
 * @ingroup System
 * @brief Definition of Mutexed Counter class using reentrant mutex
 */

#ifndef __ATOMIC_REENTRANT_COUNTER_H__
#define __ATOMIC_REENTRANT_COUNTER_H__

#include <System/Config.h>
#include <System/ReentrantMutex.h>

namespace Omiscid {

/**
 * @class AtomicReentrantCounter AtomicReentrantCounter.cpp System/AtomicReentrantCounter.h
 * @brief Counter with access protected by mutex.
 *
 * Can be incremented, decremented and, of course, accessed.
 * It can be manipulated by multiple threads.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class AtomicReentrantCounter
{
public:
	/** @brief Constructor
	 * @param init_value [in] the initial value for the counter (0 by default).
	 */
	AtomicReentrantCounter(int init_value = 0);

	// Virtual destructor always
	virtual ~AtomicReentrantCounter();

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

private:
	ReentrantMutex mutex; /*!< the mutex to protect the access */
	int counter; /*!< the integer used to store the counter value*/
};

} // namespace Omiscid

#endif // __ATOMIC_REENTRANT_COUNTER_H__
