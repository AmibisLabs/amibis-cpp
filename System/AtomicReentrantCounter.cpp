#include <System/AtomicReentrantCounter.h>

using namespace Omiscid;

AtomicReentrantCounter::AtomicReentrantCounter(int init_value)
{
	mutex.EnterMutex();
	counter = init_value;
	mutex.LeaveMutex();
}
	
	
int AtomicReentrantCounter::operator++()
{
	mutex.EnterMutex();
	++counter;
	mutex.LeaveMutex();
	return counter;
}

int AtomicReentrantCounter::operator++(int)
{
	mutex.EnterMutex();
	int val = counter++;	
	mutex.LeaveMutex();
	return val;
}

int AtomicReentrantCounter::operator--()
{
	mutex.EnterMutex();
	--counter;
	mutex.LeaveMutex();
	return counter;
}

int AtomicReentrantCounter::operator--(int)
{
	mutex.EnterMutex();	
	int val = counter--;
	mutex.LeaveMutex();
	return val;
}

AtomicReentrantCounter::operator int() const
{ return counter; }

int AtomicReentrantCounter::operator=(int value)
{
  mutex.EnterMutex();
  counter = value;
  mutex.LeaveMutex();
  return value;
}
