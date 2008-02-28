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
	SL_mutex.Lock();
	counter = init_value;
	SL_mutex.Unlock();
}

AtomicReentrantCounter::~AtomicReentrantCounter()
{
}

int AtomicReentrantCounter::operator++()
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	++counter;
	SL_mutex.Unlock();
	return counter;
}

int AtomicReentrantCounter::operator++(int)
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	int val = counter++;
	SL_mutex.Unlock();
	return val;
}

int AtomicReentrantCounter::operator--()
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	--counter;
	SL_mutex.Unlock();
	return counter;
}

int AtomicReentrantCounter::operator--(int)
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	int val = counter--;
	SL_mutex.Unlock();
	return val;
}

AtomicReentrantCounter::operator int() const
{
	return counter;
}

int AtomicReentrantCounter::operator=(int value)
{
	SmartLocker SL_mutex(mutex);
	SL_mutex.Lock();
	counter = value;
	SL_mutex.Unlock();
	return value;
}
