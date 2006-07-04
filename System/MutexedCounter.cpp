#include <System/AtomicReentrantMutex.h>

using namespace Omiscid;

AtomicReentrantMutex::AtomicReentrantMutex(int init_value)
{
	mutex.EnterMutex();
	counter = init_value;
	mutex.LeaveMutex();
}
	
	
int AtomicReentrantMutex::operator++()
{
	mutex.EnterMutex();
	++counter;
	mutex.LeaveMutex();
	return counter;
}

int AtomicReentrantMutex::operator++(int)
{
	mutex.EnterMutex();
	int val = counter++;	
	mutex.LeaveMutex();
	return val;
}

int AtomicReentrantMutex::operator--()
{
	mutex.EnterMutex();
	--counter;
	mutex.LeaveMutex();
	return counter;
}

int AtomicReentrantMutex::operator--(int)
{
	mutex.EnterMutex();	
	int val = counter--;
	mutex.LeaveMutex();
	return val;
}

AtomicReentrantMutex::operator int() const
{ return counter; }

int AtomicReentrantMutex::operator=(int value)
{
  mutex.EnterMutex();
  counter = value;
  mutex.LeaveMutex();
  return value;
}
