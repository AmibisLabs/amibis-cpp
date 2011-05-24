/**
 * @file System/SharedMemSegment.h
 * @ingroup System
 * @brief Definition of SharedMemSegment class
 */

#ifndef __SHARED_MEM_SEGMENT_H__
#define __SHARED_MEM_SEGMENT_H__

#include <System/ConfigSystem.h>
#include <System/Portage.h>
#include <System/SimpleException.h>
#include <System/SimpleString.h>

#ifndef WIN32
	// Add non windows incluse
	#include <sys/ipc.h>
	#include <sys/shm.h>
#endif

namespace Omiscid {

class SharedMemSegment
{
public:
	SharedMemSegment();
	~SharedMemSegment();

	bool CreateAndOpen( unsigned int SegtKey, int SegmentSize, bool GrantWriteAccessToOthers = false );
	bool Open( unsigned int SegtKey, bool ReadAndWriteAccess = false );
	void Close();

	void * GetSegmentAddress()
	{
		return UserAddress;
	}

protected:
	void SetName( unsigned int SegtKey );

#ifndef WIN32 // for linux and OS X
	static bool RemoveSharedMemSegment( unsigned int SegtKey );
#endif


	// Segment Management
	void * SegAddress;
	void * UserAddress;

	enum SharedMemSegmentFlags
	{
		SharedMemSegment_None		 = 0x00000000,
		SharedMemSegment_Initialized = 0x00000001,
		SharedMemSegment_ReadOnly	 = 0x00000002
	};

	SharedMemSegmentFlags * AccessFlags;

	int GetSizeOfReservedSpace()
	{
		return sizeof(SharedMemSegmentFlags);
	}

	void InitNewSharedMemSegment( bool GrantWriteAccess );
	bool GetSharedMemSegmentAccess( bool WantWriteAccess );

#ifdef WIN32
	SimpleString Name;
	HANDLE hSegMemFile;
#else
	key_t Name;
	int iSegMem;
#endif // WIN32

	int SegmentKey; // Kay given by the call to CreateAndOpen or Open
};


} // namespace Omiscid

#endif // __SHARED_MEM_SEGMENT_H__

