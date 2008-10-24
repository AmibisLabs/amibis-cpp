/**
* @file System/AtomicCounter.cpp
* @ingroup System
* @brief Implementation of AtomicCounter class
*/

#include <System/AtomicCounter.h>

#include <System/LockManagement.h>

using namespace Omiscid;

/** @brief Constructor
 *  @param init_value [in] the initial value for the counter (0 by default).
 */
AtomicCounter::AtomicCounter(int init_value)
{
	SmartLocker SL_mutex(mutex);

	counter = init_value;
}

/** @brief Virtual destructor always
 */
AtomicCounter::~AtomicCounter()
{
}

/** @brief Prefix increment operator
 *
 * Increment the integer value and next return the new value like
 * in this C code :
 * @code AtomicCounter i(0);
 * integer j;
 * j = ++i; // j == 1
 * @endcode
 */
int AtomicCounter::operator++()
{
	SmartLocker SL_mutex(mutex);

	++counter;

	return counter;
}

/** @brief Postfix increment operator
 *
 * Increment the value of the AtomicCounter and then return the previous
 * value like in this C code :
 * @code AtomicCounter i(0);
 * integer j;
 * j = i++; // j == 0
 * @endcode
 */
int AtomicCounter::operator++(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter++;

	return val;
}

/** @brief Prefix decrement operator
 *
 * Decrement the value of the AtomicCounter and then return the new
 * value like in this C code :
 * @code AtomicCounter i(0);
 * integer j;
 * j = --i; // j == -1
 * @endcode
 */
int AtomicCounter::operator--()
{
	SmartLocker SL_mutex(mutex);

	--counter;

	return counter;
}

/** @brief Postfix decrement operator
 *
 * Decrement the value of the AtomicCounter and then return the previous
 * value like in this C code :
 * @code AtomicCounter i(0);
 * integer j;
 * j = i--; // j == 0
 * @endcode
 */
int AtomicCounter::operator--(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter--;

	return val;
}

/** @brief cast operator : return the counter value as a const interger
 *
 */
AtomicCounter::operator int() const
{
	SmartLocker SL_mutex(mutex);

	return counter;
}

/** @brief equal operator to set the counter value to 'value'
 *
 * @param value [in] The new value to set for this AtomicCounter
 */
int AtomicCounter::operator=(int value)
{
	SmartLocker SL_mutex(mutex);

	counter = value;

	return value;
}
