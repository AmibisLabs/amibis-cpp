#include <Com/Message.h>

#include <stdlib.h>

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
  
