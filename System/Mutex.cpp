// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////


#include <System/Mutex.h>
#include <System/SimpleException.h>

using namespace Omiscid;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Mutex::Mutex()
{
#ifdef WIN32
	// mutex = CreateMutex( NULL, false, NULL );
	mutex = CreateSemaphore(NULL, 1, (LONG)0xffff, NULL );
	#ifdef DEBUG
		if ( mutex == NULL )
		{
			int err = GetLastError();
			OmiscidError( "Could not create mutex : %d\n", err );
		}
		OwnerId = 0;
	#endif
#else
	if ( pthread_mutex_init(&mutex, NULL) != 0 )
	{
		throw  SimpleException("Error Mutex Init");
	}
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	if ( mutex )
	{
		CloseHandle( mutex );
	}
#else
	pthread_mutex_destroy(&mutex);
#endif
}

bool Mutex::EnterMutex()
{
#ifdef WIN32
	unsigned int Result = WaitForSingleObject( mutex, INFINITE );
	if ( Result == WAIT_OBJECT_0 )
	{
#ifdef DEBUG
		// In debug mode, set the owner id
		OwnerId = GetCurrentThreadId();
#endif
		// Here we go, we've got the mutex
		return true;
	}
#else
	if( pthread_mutex_lock(&mutex) == 0 )
	{
		return true;
	}
#endif

	return false;
}

bool Mutex::LeaveMutex()
{
#ifdef WIN32
	// if ( !ReleaseMutex(mutex) )
	if ( !ReleaseSemaphore(mutex,1,NULL) )
	{
		return false;
	}
	#ifdef DEBUG
		OwnerId = 0;
	#endif

#else
	if( pthread_mutex_unlock(&mutex) != 0 )
	{
		return false;
	}
#endif

	return true;
}
