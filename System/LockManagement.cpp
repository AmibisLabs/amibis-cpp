// LockableObject.cpp: implementation of the  class.
//
//////////////////////////////////////////////////////////////////////


#include <System/LockManagement.h>

using namespace Omiscid;

	/** @brief Constructor
	 *
	 */
SmartLocker::SmartLocker( LockableObject& LockableObjectToManage )
	: ManagedLoackableObject(LockableObjectToManage)
{
	// Initiate the LockCount
	LockCount = 0;

	// Here, we are ready to work
}

	/** @brief Constructor
	 *
	 */
SmartLocker::~SmartLocker()
{
	if ( LockCount > 0 )
	{
		OmiscidError( "SmartLocker::SmartLocker~: warning object not unlock before exiting the scope\n" );
		while( LockCount > 0 )
		{
			ManagedLoackableObject.Unlock();
			LockCount--;
		}
	}
}

	/** @brief Function used to lock the LockableObject object
	 *
	 */
bool SmartLocker::Lock()
{
	if ( ManagedLoackableObject.Lock() == true )
	{
		LockCount++;
		return true;
	}
	return false;
}

	/** @brief Function used to unlock the LockableObject object
	 *
	 */
bool SmartLocker::Unlock()
{
	if ( LockCount == 0 )
	{
		OmiscidError( "SmartLocker::Unlock : the object is not lock." );
		return false;
	}
	if ( ManagedLoackableObject.Unlock() == true )
	{
		LockCount--;
		return true;
	}
	return false;
}
