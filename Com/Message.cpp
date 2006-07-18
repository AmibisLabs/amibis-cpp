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
  
char* Message::GetBuffer() const
{
	return buffer; 
}

int Message::GetLength() const
{
	return len;
}

MessageOrigine Message::GetOrigine() const
{
	return origine;
}

unsigned int Message::GetPeerId() const
{
	return pid; 
}

unsigned int Message::GetMsgId() const
{
	return mid;
}
