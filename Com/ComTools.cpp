#include <Com/ComTools.h>
#include <System/Portage.h>

using namespace Omiscid;

// test commit

unsigned short ComTools::GetTcpPort()
{
  return 0;
}
unsigned short ComTools::GetUdpPort()
{
  return 0;
}

ComTools::~ComTools()
{
}

unsigned int ComTools::GeneratePeerId()
{
	unsigned int res;

	res = random();

	return (res & SERVICE_PEERID);
}

unsigned int ComTools::PeerIdFromString(const SimpleString& StringPeerId)
{
	unsigned int res;

	if ( StringPeerId.IsEmpty() )
	{
		return 0;
	}

	if ( sscanf( StringPeerId.GetStr(), "%x", &res ) == 1 )
	{
		return res;
	}
	return 0;
}

SimpleString ComTools::PeerIdAsString(unsigned int PeerId)
{
	TemporaryMemoryBuffer tmp(30);

	(*(char*)tmp) = '\0';

	sprintf( (char*)tmp, "%8.8x", PeerId );

	return SimpleString(tmp);
}

// const SimpleString ComTools::MagicUdp("udp-port");

SimpleString ComTools::ValueFromKey(const SimpleString array, const SimpleString key)
{
   unsigned int klen = key.GetLength();	// key length
   unsigned int len  = array.GetLength (); // remaining characters
   unsigned int idx  = 0;				   // index of the current character
   unsigned int idx_start = 0;			  // index of the first data character

   line_start:

	 if (len < klen + 1)
	 {
		 goto eat_line;
	 }

	 // check if key matches the line head
	 if ( key != array.SubString(idx, idx + klen) )
	 {
		 goto eat_line;
	 }
	 idx += klen; len -= klen;

	 // check if a colon follows immediately
	 if (array[idx] != ':')
	 {
		 goto eat_line;
	 }
	 idx += 1; len -= 1;

   eat_spaces:

	 if (len == 0)
	 {
		 return SimpleString::EmptyString();
	 }

	 switch (array[idx])
	 {
	   case ' ':
	   case '\t':
		 idx += 1; len -= 1;
		 goto eat_spaces;

	   default:
		 break;
	 }
	 idx_start = idx;
	 idx += 1; len -= 1;

   eat_data:

	 if (len == 0)
	 {
		 goto data_eaten;
	 }

	 switch (array[idx])
	 {
	   case '\r':
	   case '\n':
		 goto data_eaten;
	   default:
		 idx += 1; len -= 1;
		 goto eat_data;
	 }

   data_eaten:
	 return array.SubString(idx_start, idx);

   eat_line:

	 if (len == 0)
	 {
		 return SimpleString::EmptyString();
	 }

	 switch (array[idx])
	 {
	   case '\n':
		 idx += 1; len -= 1;
		 goto line_start;

	   default:
		 idx += 1; len -= 1;
		 goto eat_line;
	 }

	 // We never come here
	 // return SimpleString::EmptyString;
}
