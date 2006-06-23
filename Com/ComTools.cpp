#include <Com/ComTools.h>

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
