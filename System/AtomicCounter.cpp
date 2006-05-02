#include <System/AtomicCounter.h>


AtomicCounter::AtomicCounter(int init_value)
{
	mutex.EnterMutex();
	counter = init_value;
	mutex.LeaveMutex();
}
	
	
int AtomicCounter::operator++()
{
	mutex.EnterMutex();
	++counter;
	mutex.LeaveMutex();
	return counter;
}

int AtomicCounter::operator++(int)
{
	mutex.EnterMutex();
	int val = counter++;	
	mutex.LeaveMutex();
	return val;
}

int AtomicCounter::operator--()
{
	mutex.EnterMutex();
	--counter;
	mutex.LeaveMutex();
	return counter;
}

int AtomicCounter::operator--(int)
{
	mutex.EnterMutex();	
	int val = counter--;
	mutex.LeaveMutex();
	return val;
}

AtomicCounter::operator int() const
{ return counter; }

int AtomicCounter::operator=(int value)
{
  mutex.EnterMutex();
  counter = value;
  mutex.LeaveMutex();
  return value;
}
