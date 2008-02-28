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
	SL_mutex.Lock();
	counter = init_value;
	SL_mutex.Unlock();
}

AtomicCounter::~AtomicCounter()
{
}

int AtomicCounter::operator++()
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	++counter;
	SL_mutex.Unlock();
	return counter;
}

int AtomicCounter::operator++(int)
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	int val = counter++;
	SL_mutex.Unlock();
	return val;
}

int AtomicCounter::operator--()
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	--counter;
	SL_mutex.Unlock();
	return counter;
}

int AtomicCounter::operator--(int)
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	int val = counter--;
	SL_mutex.Unlock();
	return val;
}

AtomicCounter::operator int() const
{
	return counter;
}

int AtomicCounter::operator=(int value)
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	counter = value;
	SL_mutex.Unlock();

	return value;
}
