/**
* @file System/AtomicCounter.cpp
* @ingroup System
* @brief Implementation of AtomicCounter class
*/

#include <System/AtomicCounter.h>

#include <System/LockManagement.h>

using namespace Omiscid;

AtomicCounter::AtomicCounter(int init_value)
{
	SmartLocker SL_mutex(mutex);

	counter = init_value;
}

AtomicCounter::~AtomicCounter()
{
}

int AtomicCounter::operator++()
{
	SmartLocker SL_mutex(mutex);

	++counter;

	return counter;
}

int AtomicCounter::operator++(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter++;

	return val;
}

int AtomicCounter::operator--()
{
	SmartLocker SL_mutex(mutex);

	--counter;

	return counter;
}

int AtomicCounter::operator--(int)
{
	SmartLocker SL_mutex(mutex);

	int val = counter--;

	return val;
}

AtomicCounter::operator int() const
{
	SmartLocker SL_mutex(mutex);

	return counter;
}

int AtomicCounter::operator=(int value)
{
	SmartLocker SL_mutex(mutex);

	counter = value;

	return value;
}
