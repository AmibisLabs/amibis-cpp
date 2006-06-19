#include <System/MutexedCounter.h>


MutexedCounter::MutexedCounter(int init_value)
{
	mutex.EnterMutex();
	counter = init_value;
	mutex.LeaveMutex();
}
	
	
int MutexedCounter::operator++()
{
	mutex.EnterMutex();
	++counter;
	mutex.LeaveMutex();
	return counter;
}

int MutexedCounter::operator++(int)
{
	mutex.EnterMutex();
	int val = counter++;	
	mutex.LeaveMutex();
	return val;
}

int MutexedCounter::operator--()
{
	mutex.EnterMutex();
	--counter;
	mutex.LeaveMutex();
	return counter;
}

int MutexedCounter::operator--(int)
{
	mutex.EnterMutex();	
	int val = counter--;
	mutex.LeaveMutex();
	return val;
}

MutexedCounter::operator int() const
{ return counter; }

int MutexedCounter::operator=(int value)
{
  mutex.EnterMutex();
  counter = value;
  mutex.LeaveMutex();
  return value;
}
