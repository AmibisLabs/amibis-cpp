
#include <System/TrackingMemoryLeaks.h>

#include <list>
using namespace std;

#if defined WIN32 || defined _WIN32
	#include <windows.h>
	#include <winbase.h>

	#define snprintf _snprintf
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

typedef enum TrackMemoryValues{
	FileNameSize = 255,
	TemporaryBufferSize = 4*1024-1	// 4 Kb - 1 byte
};

void DumpUnfreed();


static bool Tracking = false;

class MemoryBlockInfos
{
public:
	char	Filename[FileNameSize+1];
	void*	Where;
	size_t	Size;
	int		Line;
};

typedef list<MemoryBlockInfos*> AllocList;
static AllocList *allocList;

static char WriteSizeBuffer[TemporaryBufferSize+1];

static char * PrintSize( unsigned int SizeOfData );

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

	if ( Tracking == false )
	{
		return;
	}

	if( allocList == NULL )
	{
		allocList = new(AllocList);
	}

	if( allocList == NULL )
	{
		// Could not allocate list, disable tracking
		Tracking = false;
		return;
	}

	info = new MemoryBlockInfos;

	if ( info == NULL )
	{
		// Could not allocate list, disable tracking
		Tracking = false;
		return;

	}

	// Fill information
	info->Where = addr;
	strncpy( info->Filename, fname, FileNameSize-1 );
	info->Filename[FileNameSize-1];
	info->Line = lnum;
	info->Size = aSize;
	allocList->insert(allocList->begin(), info);
}

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
}

static char * Omiscid::PrintSize( unsigned int SizeOfData )
{
	if ( SizeOfData < 1024 )
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%d bytes", SizeOfData);
	}
	else if ( SizeOfData < 1024*1024 )
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%.2f Kbytes", (float)SizeOfData/1024.0f);
	}
	else if ( SizeOfData < 1024*1024*1024 )
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%.2f Mbytes", (float)SizeOfData/(1024.0f*1024.0f));
	}
	else // totalSize >= 1024*1024*1024*1024
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%.2f Gbytes", (float)SizeOfData/(1024.0f*1024.0f*1024.0f));
	}

	return WriteSizeBuffer;
}

void Omiscid::DumpUnfreed()
{
	AllocList::iterator i;
	unsigned int TotalSize = 0;
	char buf[TemporaryBufferSize+1];	     

	if( allocList == NULL )
		return;

	if ( allocList->size() == 0 )
	{
		snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\nNo memory leak detected\n-+-+-+-+-+-+-+-+\n" );
		OutputDebugString(buf);
		return;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	for(i = allocList->begin(); i != allocList->end(); i++) 
	{
		// Visual studio compliant output (you can click on it to see the rigth line
		snprintf(buf, TemporaryBufferSize+1, "%s(%d) : %s unfreed memory at address %p\n", (*i)->Filename, (*i)->Line, PrintSize((unsigned int)(*i)->Size), (*i)->Where );
		OutputDebugString(buf);

		void * tmpv = (*i)->Where;
		char * tmpc = (char*)tmpv;
		int lSize = (int)(*i)->Size;
		for( int j = 0; j < lSize && j < 20; j++)
		{
			snprintf(buf, TemporaryBufferSize+1, "%c", tmpc[j] );
			OutputDebugString(buf);
		}
		snprintf(buf, TemporaryBufferSize+1, "\n" );
		OutputDebugString(buf);

		TotalSize += (unsigned int)lSize;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);
	
	snprintf(buf, TemporaryBufferSize+1, "Total memory unfreed %s.\n", PrintSize(TotalSize) );
	OutputDebugString(buf);

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);
}

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

