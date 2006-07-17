
#include <System/TrackingMemoryLeaks.h>

#include <list>
using namespace std;

#if defined WIN32 || defined _WIN32
	#include <windows.h>
	#include <winbase.h>
#else
namespace Omiscid {
	inline void OutputDebugString(const char * OutputString)
	{
		if ( OutputString )
		{
			fprintf( stderr, OutputString );
		}
	}
} // namespace Omiscid
#endif

// Local Omiscid declaration
namespace Omiscid {

void DumpUnfreed();


static bool Tracking = false;

class MemoryBlockInfos
{
public:
	void*	Where;
	size_t	Size;
	char	Filename[256];
	int		Line;
};

typedef list<MemoryBlockInfos*> AllocList;
static AllocList *allocList;

} // namespace Omiscid

using namespace Omiscid;

void Omiscid::StartTrackingMemoryLeaks()
{
	Tracking = true;
}

void Omiscid::StopTrackingMemoryLeaks()
{
	Tracking = false;
}

void Omiscid::AddMemoryBlock(void* addr,  size_t aSize,  const char *fname, int lnum)
{
	MemoryBlockInfos *info;

	if ( ! Tracking )
	{
		return;
	}

	if(!allocList)
	{
		allocList = new(AllocList);
	}
	info = new(MemoryBlockInfos);
	info->Where = addr;
	strncpy(info->Filename, fname, 127);
	info->Line = lnum;
	info->Size = aSize;
	allocList->insert(allocList->begin(), info);
};

void Omiscid::RemoveMemoryBlock(void* addr)
{
	AllocList::iterator i;

	if(!allocList)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++)
	{
		if((*i)->Where == addr)
		{
			allocList->remove((*i));
			break;
		}
	}
};

void Omiscid::DumpUnfreed()
{
	AllocList::iterator i;
	unsigned int totalSize = 0;
	char buf[1024];	     

	if(!allocList)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++) 
	{
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %p\t%u unfreed\n",
			(*i)->Filename, (*i)->Line, (*i)->Where, (*i)->Size);
		OutputDebugString(buf);

		void * tmpv = (*i)->Where;
		char * tmpc = (char*)tmpv;
		int taille = (int)(*i)->Size;
		for( int j = 0; j < taille && j < 20; j++)
		{
			sprintf(buf, "%c", tmpc[j] );
			OutputDebugString(buf);
		}
		sprintf(buf, "\n" );
		OutputDebugString(buf);

		totalSize += (unsigned int)taille;
	}
	sprintf(buf, "-+-+-+-\n");
	OutputDebugString(buf);
	if ( totalSize < 1024 )
	{
		sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	}
	else if ( totalSize < 1024*1024 )
	{
		sprintf(buf, "Total Unfreed: %.2f Kbytes\n", (float)totalSize/1024.0f);
	}
	else if ( totalSize < 1024*1024*1024 )
	{
		sprintf(buf, "Total Unfreed: %.2f Mbytes\n", (float)totalSize/(1024.0f*1024.0f));
	}
	else // totalSize >= 1024*1024*1024*1024
	{
		sprintf(buf, "Total Unfreed: %.2f Mbytes\n", (float)totalSize/(1024.0f*1024.0f*1024.0f));
	}
	OutputDebugString(buf);
};

namespace Omiscid {

class UnfreedPrint
{
public:
	~UnfreedPrint()
	{
		DumpUnfreed();
	}
};

static UnfreedPrint PrintUnfreed;

} // namespace Omiscid
