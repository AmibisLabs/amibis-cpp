#include <stdlib.h>
#include <Com/Message.h>

using namespace Omiscid;

Message::Message(int size)
{
  realBuffer = NULL;
  buffer = NULL;
  len = 0;
  origUdp = false;

  realBuffer = new char[size+1];
  buffer = realBuffer+1;
}

Message::~Message()
{
  delete [] realBuffer;
}
  
char* Message::GetBuffer() 
{
	return buffer; 
}

int Message::GetLength()
{
	return len;
}

bool Message::GetOrigUdp()
{
	return origUdp; 
}

unsigned int Message::GetPid() const
{
	return pid; 
}

unsigned int Message::GetMid() const
{
	return mid;
}
