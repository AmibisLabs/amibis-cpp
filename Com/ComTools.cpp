#include <Com/ComTools.h>
#include <System/Portage.h>

#include <stdlib.h>

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

namespace Omiscid {

class OmiscidRandomInitClass
{
public:
	OmiscidRandomInitClass()
	{
		struct timeval t;    
		gettimeofday(&t, NULL);

#ifdef WIN32
		srand(t.tv_sec ^ t.tv_usec);
#else
		srandom(t.tv_sec ^ t.tv_usec);
#endif
	};
};

do peerid !

static OmiscidRandomInitClass OmiscidRandomInitClassInitialisationObject;

} // namespace Omiscid

unsigned int ComTools::GeneratePeerId()
{
	struct timeval t;    
	unsigned int res;

	gettimeofday(&t, NULL);

	// Hope we will go out
	for(;;)
	{
		res = t.tv_sec << 16;

#ifdef WIN32
		res += (0x0000FFFF & rand());
#else
		res += (0x0000FFFF & random());
#endif

		// Do we manage to generate a good service id
		if ( (res & SERVICE_PEERID) != 0 )
		{
			return res & SERVICE_PEERID;
		}
	}
}

const SimpleString ComTools::MagicUdp("udp-port");

SimpleString * ComTools::ValueFromKey(SimpleString& array, SimpleString& key)
{
   unsigned int klen = key.GetLength();    // key length
   unsigned int len  = array.GetLength (); // remaining characters
   unsigned int idx  = 0;                   // index of the current character
   unsigned int idx_start = 0;              // index of the first data character

   line_start:

     if (len < klen + 1)
	 {
		 goto eat_line;
	 }

     // check if key matches the line head
     if (key != array.SubString(idx, idx + klen))
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
		 return NULL;
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
     return new SimpleString( array.SubString(idx_start, idx) );

   eat_line:

     if (len == 0)
	 {
		 return NULL;
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

	 return NULL;
}
