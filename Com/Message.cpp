#include <Com/Message.h>

using namespace Omiscid;

Message::Message(int size)
{
  realBuffer = NULL;
  buffer = NULL;
  len = 0;
  origine = UnknownOrigine;
  OriginalSize = size;

  if ( OriginalSize > 0 )
  {
	realBuffer = new char[size+1];
	buffer = realBuffer+1;

  }
}

Message::~Message()
{
	if ( OriginalSize != 0 )
	{
		delete [] realBuffer;
	}
}
  
char* Message::GetBuffer() 
{
	return buffer; 
}

int Message::GetLength()
{
	return len;
}

MessageOrigine Message::GetOrigine()
{
	return origine;
}

unsigned int Message::GetPid() const
{
	return pid; 
}

unsigned int Message::GetMid() const
{
	return mid;
}
