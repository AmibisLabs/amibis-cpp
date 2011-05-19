/**
 * @file System/System/AtomicCounter.h
 * @ingroup System
 * @brief Definition of AtomicCounter class
 */

#ifndef __ATOMIC_COUNTER_H__
#define __ATOMIC_COUNTER_H__

#include <System/ConfigSystem.h>
#include <System/Mutex.h>

namespace Omiscid {

/**
 * @class AtomicCounter AtomicCounter.cpp System/AtomicCounter.h
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
	 *  @param init_value [in] the initial value for the counter (0 by default).
	 */
	AtomicCounter(int init_value = 0);

	/** @brief Copy constructor
	 *  @param ToCopy [in] The AtomicCounter to copy
	 */
	AtomicCounter(const AtomicCounter& ToCopy);

	/** @brief Virtual destructor always
	 */
	virtual ~AtomicCounter();

	/** @name Operators */
	//@{
	/** @brief Prefix increment operator
	 *
	 * Increment the integer value and next return the new value like
	 * in this C code :
	 * @code AtomicCounter i(0);
	 * integer j;
	 * j = ++i; // j == 1
	 * @endcode
	 */
	int operator++();

	/** @brief Equal operator
	 *  @param ToCopy [in] The AtomicCounter to copy
	 */
	const AtomicCounter& operator=(const AtomicCounter& ToCopy);

	/** @brief Postfix increment operator
	 *
	 * Increment the value of the AtomicCounter and then return the previous
	 * value like in this C code :
	 * @code AtomicCounter i(0);
	 * integer j;
	 * j = i++; // j == 0
	 * @endcode
	 */
	int operator++(int);

	/** @brief Prefix decrement operator
	 *
	 * Decrement the value of the AtomicCounter and then return the new
	 * value like in this C code :
	 * @code AtomicCounter i(0);
	 * integer j;
	 * j = --i; // j == -1
	 * @endcode
	 */
	int operator--();

	/** @brief Postfix decrement operator
	 *
	 * Decrement the value of the AtomicCounter and then return the previous
	 * value like in this C code :
	 * @code AtomicCounter i(0);
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
	//@}

private:
	Mutex mutex; /*!< the mutex to protect the access */
	int counter; /*!< the integer used to store the counter value*/
};

} // namespace Omiscid

#endif // __ATOMIC_COUNTER_H__
