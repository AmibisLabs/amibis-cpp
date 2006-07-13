#if defined WIN32 || defined _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>

#if defined WIN32 || defined _WIN32
	#include <windows.h>
	#include <winbase.h>
#else
	inline OutputDebugString(const char * OutputString)
	{
		if ( OutputString )
		{
			fprintf( stderr, OutputString );
		}
	}
#endif


#include <System/TrackingMemoryLeaks.h>

#include <list>


using namespace std;

static bool Tracking = false;

class MemoryBlockInfos
{
public:
	void*	address;
	unsigned int	size;
	char	file[128];
	int	line;
};

typedef list<MemoryBlockInfos*> AllocList;
AllocList *allocList;

void StartTrackingMemoryLeaks()
{
	Tracking = true;
}

void StopTrackingMemoryLeaks()
{
	Tracking = false;
}

void AddTrack(void* addr,  unsigned int asize,  const char *fname, int lnum)
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
	info->address = addr;
	strncpy(info->file, fname, 127);
	info->line = lnum;
	info->size = asize;
	allocList->insert(allocList->begin(), info);
};

void RemoveTrack(void* addr)
{
	AllocList::iterator i;

	if(!allocList)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++)
	{
		if((*i)->address == addr)
		{
			allocList->remove((*i));
			break;
		}
	}
};

void DumpUnfreed()
{
	AllocList::iterator i;
	unsigned int totalSize = 0;
	char buf[1024];	     

	if(!allocList)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++) 
	{
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %p\t%u unfreed\n",
			(*i)->file, (*i)->line, (*i)->address, (*i)->size);
		OutputDebugString(buf);

		void * tmpv = (*i)->address;
		char * tmpc = (char*)tmpv;
		int taille = (*i)->size;
		for( int j = 0; j < taille && j < 20; j++)
		{
			sprintf(buf, "%c", tmpc[j] );
			OutputDebugString(buf);
		}
		sprintf(buf, "\n" );
		OutputDebugString(buf);

		totalSize += (*i)->size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
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

class UnfreedPrint
{
public:
	~UnfreedPrint()
	{
		DumpUnfreed();
	}
};

static UnfreedPrint PrintUnfreed;