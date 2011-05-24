/**
 * @file System/System/AtomicReentrantCounter.h
 * @ingroup System
 * @brief Definition of Mutexed Counter class using reentrant mutex
 */

#ifndef __ATOMIC_REENTRANT_COUNTER_H__
#define __ATOMIC_REENTRANT_COUNTER_H__

#include <System/ConfigSystem.h>
#include <System/ReentrantMutex.h>

namespace Omiscid {

/**
 * @class AtomicReentrantCounter AtomicReentrantCounter.cpp System/AtomicReentrantCounter.h
 * @brief Counter with access protected by a reentrant mutex.
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

	/** @brief Copy constructor
	 *  @param ToCopy [in] The AtomicReentrantCounter to copy
	 */
	AtomicReentrantCounter(const AtomicReentrantCounter& ToCopy);

	/** @brief Virtual destructor always
	 */
	virtual ~AtomicReentrantCounter();

	/** @name Operators */
	//@{
	/** @brief Prefix increment operator
	 *
	 * Increment the integer value and next return the new value like
	 * in this C code :
	 * @code AtomicReentrantCounter i(0);
	 * integer j;
	 * j = ++i; // j == 1
	 * @endcode
	 */
	int operator++();

	/** @brief Postfix increment operator
	 *
	 * Increment the value of the AtomicCounter and then return the previous
	 * value like in this C code :
	 * @code AtomicReentrantCounter i(0);
	 * integer j;
	 * j = i++; // j == 0
	 * @endcode
	 */
	int operator++(int);

	/** @brief Prefix decrement operator
	 *
	 * Decrement the value of the AtomicCounter and then return the new
	 * value like in this C code :
	 * @code AtomicReentrantCounter i(0);
	 * integer j;
	 * j = --i; // j == -1
	 * @endcode
	 */
	int operator--();

	/** @brief Postfix decrement operator
	 *
	 * Decrement the value of the AtomicCounter and then return the previous
	 * value like in this C code :
	 * @code AtomicReentrantCounter i(0);
	 * integer j;
	 * j = i--; // j == 0
	 * @endcode
	 */
	int operator--(int);

	/** @brief cast operator : return the counter value as const
	 *
	 */
	operator int() const;

	/** @brief equal operator to set the counter value to 'value'
	 *
	 * @param value [in] The new value to set for this AtomicCounter
	 */
	int operator=(int value);

	/** @brief Equal operator
	 *  @param ToCopy [in] The AtomicCounter to copy
	 */
	const AtomicReentrantCounter& operator=(const AtomicReentrantCounter& ToCopy);
	//@}

private:
	ReentrantMutex mutex; /*!< the mutex to protect the access */
	int counter; /*!< the integer used to store the counter value*/
};

} // namespace Omiscid

#endif // __ATOMIC_REENTRANT_COUNTER_H__

