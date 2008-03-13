/**
 * @file System/AtomicReentrantCounter.cpp
 * @ingroup System
 * @brief Definition of Mutexed Counter class using reentrant mutex
 */

#include <System/AtomicReentrantCounter.h>

#include <System/LockManagement.h>

using namespace Omiscid;

AtomicReentrantCounter::AtomicReentrantCounter(int init_value)
{
	SmartLocker SL_mutex(mutex);

	counter = init_value;
}

AtomicReentrantCounter::~AtomicReentrantCounter()
{
}

int AtomicReentrantCounter::operator++()
{
	SmartLocker SL_mutex(mutex);

	++counter;

	return counter;
}

int AtomicReentrantCounter::operator++(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter++;

	return val;
}

int AtomicReentrantCounter::operator--()
{
	SmartLocker SL_mutex(mutex);

	--counter;

	return counter;
}

int AtomicReentrantCounter::operator--(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter--;

	return val;
}

AtomicReentrantCounter::operator int() const
{
	return counter;
}

int AtomicReentrantCounter::operator=(int value)
{
	SmartLocker SL_mutex(mutex);

	counter = value;

	return value;
}
