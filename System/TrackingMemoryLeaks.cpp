
// #include <System/TrackingMemoryLeaks.h>

#ifdef NDEBUG

#if defined WIN32 || defined _WIN32
	#define _CRT_SECURE_NO_DEPRECATE

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

#include <stdio.h>
#include <stdlib.h>


// Local Omiscid declaration
namespace Omiscid {

void StartTrackingMemoryLeaks();
void StopTrackingMemoryLeaks();

void AddMemoryBlock(void* addr,  size_t asize,  const char *fname, int lnum);
void RemoveMemoryBlock(void* addr);

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
	bool	Freed;
	MemoryBlockInfos * Next;

	MemoryBlockInfos()
	{
		Filename[0] = '\0';
		Where = NULL;
		Size = 0;
		Line = -1;
		Freed = false;
		Next = NULL;
	}
};

MemoryBlockInfos * AllocList;

static char WriteSizeBuffer[TemporaryBufferSize+1];

static char * PrintSize( unsigned int SizeOfData );

} // namespace Omiscid

using namespace Omiscid;

void * operator new( size_t size, int line, const char *file )
{
	void *ptr = (void *)malloc(size);
	Omiscid::AddMemoryBlock(ptr, size, file, line);
	return(ptr);
}

void * operator new[]( size_t size, int line, const char *file )
{
	void *ptr = (void *)malloc(size);
	Omiscid::AddMemoryBlock(ptr, size, file, line);
	return(ptr);
}

void operator delete(void *p)
{
	Omiscid::RemoveMemoryBlock(p);
	free(p);
}

void operator delete[](void *p)
{
	Omiscid::RemoveMemoryBlock(p);
	free(p);
}

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
	
	// Add head
	if ( AllocList != NULL )
	{
		info->Next = AllocList;
	}
	AllocList = info;
}

void Omiscid::RemoveMemoryBlock(void* addr)
{
	MemoryBlockInfos * pTmp;

	for( pTmp = AllocList; pTmp != NULL; pTmp = pTmp->Next )
	{
		if( pTmp->Where == addr )
		{
			pTmp->Freed = true;
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
	unsigned int TotalSize = 0;
	unsigned int NbBlocks = 0;
	char buf[TemporaryBufferSize+1];	   

	if ( AllocList == NULL )
	{
		return;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	MemoryBlockInfos * pTmp;

	for( pTmp = AllocList; pTmp != NULL; pTmp = pTmp->Next )
	{
		NbBlocks++;

		if ( pTmp->Freed == true )
		{
			continue;
		}

		// Visual studio compliant output (you can click on it to see the rigth line
		snprintf(buf, TemporaryBufferSize+1, "%s(%d) : %s unfreed memory at address %p\n", pTmp->Filename, pTmp->Line, PrintSize((unsigned int)pTmp->Size), pTmp->Where );
		OutputDebugString(buf);

		void * tmpv = pTmp->Where;
		char * tmpc = (char*)tmpv;
		int lSize = (int)pTmp->Size;
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
	
	snprintf(buf, TemporaryBufferSize+1, "Total memory unfreed %s (%u allocated blocks).\n", PrintSize(TotalSize), NbBlocks );
	OutputDebugString(buf);

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	// Free the memory
	for( pTmp = AllocList; pTmp != NULL; pTmp = AllocList )
	{
		AllocList = pTmp->Next;
		delete pTmp;
	}
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

#endif

