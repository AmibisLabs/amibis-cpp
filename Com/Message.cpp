#include <Com/Message.h>

#include <System/Portage.h>
#include <Com/ComTools.h>

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
	realBuffer = new OMISCID_TLM char[size+1];
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

  /** @brief ToString
   *
   * Generate a description of the message
   */
SimpleString Message::ToString(unsigned int MaxBufferLenToWrite /* default = 20, min value = 0 max value is 256 */) const
{
	TemporaryMemoryBuffer TmpBuffer(300);

	SimpleString TmpMessage = "Message ";
	TmpMessage	 += mid;
	TmpMessage	 += " (";

	if ( len < 1024 )	// less that 1 KB
	{
		TmpMessage += len;
		TmpMessage += "bytes)";
	}
	else if ( len >= 1024 && len < (1024*1024) ) // from 1KB to 1Mo
	{
		// compute size in ko
		*((char*)TmpBuffer) = '\0';	// Empty string
		snprintf( (char*)TmpBuffer, TmpBuffer.GetLength(), "%.2f Kbytes)", ((float)len)/1024.f );
		TmpMessage += (char*)TmpBuffer;
	}
	else	// over 1 Mo
	{
		// compute size in ko
		*((char*)TmpBuffer) = '\0';	// Empty string
		snprintf( (char*)TmpBuffer, TmpBuffer.GetLength(), "%.2f Mbytes)", ((float)len)/(1024.0f*1024.0f) );
		TmpMessage += (char*)TmpBuffer;
	}

	if ( origine == FromUDP )
	{
		TmpMessage += " on UDP from ";
	}
	else
	{
		TmpMessage += " on TCP from ";
	}

	TmpMessage += ComTools::PeerIdAsString(pid);

	if ( MaxBufferLenToWrite == 0 || MaxBufferLenToWrite > 256 )
	{
		TmpMessage += "\n";
	}
	else
	{
		SimpleString Format = ":\n%";
		Format += MaxBufferLenToWrite;
		Format += ".";
		Format += MaxBufferLenToWrite;
		Format += "s\n";
		*((char*)TmpBuffer) = '\0';	// Empty string
		snprintf( (char*)TmpBuffer, TmpBuffer.GetLength(), (const char*)Format.GetStr(), buffer );
		TmpMessage += (char*)TmpBuffer;
	}

	return TmpMessage;
}
