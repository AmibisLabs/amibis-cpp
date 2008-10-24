/**
 * @file System/AtomicReentrantCounter.cpp
 * @ingroup System
 * @brief Definition of Mutexed Counter class using reentrant mutex
 */

#include <System/AtomicReentrantCounter.h>

#include <System/LockManagement.h>

using namespace Omiscid;

/** @brief Constructor
 * @param init_value [in] the initial value for the counter (0 by default).
 */
AtomicReentrantCounter::AtomicReentrantCounter(int init_value /* = 0 */)
{
	SmartLocker SL_mutex(mutex);

	counter = init_value;
}

/** @brief Virtual destructor always
 */
AtomicReentrantCounter::~AtomicReentrantCounter()
{
}

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
int AtomicReentrantCounter::operator++()
{
	SmartLocker SL_mutex(mutex);

	++counter;

	return counter;
}

/** @brief Postfix increment operator
 *
 * Increment the value of the AtomicCounter and then return the previous
 * value like in this C code :
 * @code AtomicReentrantCounter i(0);
 * integer j;
 * j = i++; // j == 0
 * @endcode
 */
int AtomicReentrantCounter::operator++(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter++;

	return val;
}

/** @brief Prefix decrement operator
 *
 * Decrement the value of the AtomicCounter and then return the new
 * value like in this C code :
 * @code AtomicReentrantCounter i(0);
 * integer j;
 * j = --i; // j == -1
 * @endcode
 */
int AtomicReentrantCounter::operator--()
{
	SmartLocker SL_mutex(mutex);

	--counter;

	return counter;
}

/** @brief Postfix decrement operator
 *
 * Decrement the value of the AtomicCounter and then return the previous
 * value like in this C code :
 * @code AtomicReentrantCounter i(0);
 * integer j;
 * j = i--; // j == 0
 * @endcode
 */
int AtomicReentrantCounter::operator--(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter--;

	return val;
}

/** @brief cast operator : return the counter value as const
 *
 */
AtomicReentrantCounter::operator int() const
{
	return counter;
}

/** @brief equal operator to set the counter value to 'value'
 *
 * @param value [in] The new value to set for this AtomicCounter
 */
int AtomicReentrantCounter::operator=(int value)
{
	SmartLocker SL_mutex(mutex);

	counter = value;

	return value;
}
