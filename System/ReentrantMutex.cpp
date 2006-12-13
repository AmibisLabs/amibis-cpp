// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////


#include <System/ReentrantMutex.h>
#include <System/SimpleException.h>

using namespace Omiscid;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ReentrantMutex::ReentrantMutex()
{
#ifdef WIN32
	mutex = CreateMutex(NULL, false, NULL );
	#ifdef DEBUG
		OwnerId = 0;
	#endif
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init (&mutex, &attr);

	if(pthread_mutex_init(&mutex, &attr) != 0)
		throw  SimpleException("Error Mutex Init");
#endif	
}

ReentrantMutex::~ReentrantMutex()
{
#ifdef WIN32
	if ( mutex )
	{
		CloseHandle( mutex );
	}
#else
	int err = pthread_mutex_destroy(&mutex);
	switch( err )
	{
		case 0:	// no error
			break;
	
		case EINVAL:
			throw SimpleException("Error ReentrantMutex Destroy (EINVAL)", err );
			break;

		case EBUSY:
			throw SimpleException("Error ReentrantMutex Destroy (EBUSY)", err );
			break;

		default:
			throw SimpleException("Error ReentrantMutex Destroy (unkown error)", err );
			break;
	}
#endif
}

bool ReentrantMutex::EnterMutex()
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

bool ReentrantMutex::LeaveMutex()
{
#ifdef WIN32
	if ( !ReleaseMutex(mutex) )
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
