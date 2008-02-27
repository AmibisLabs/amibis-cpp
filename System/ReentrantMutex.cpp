// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////


#include <System/ReentrantMutex.h>
#include <System/SimpleException.h>
#include <System/Thread.h>

using namespace Omiscid;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ReentrantMutex::ReentrantMutex()
{
#ifdef WIN32
	mutex = CreateMutex(NULL, false, NULL );
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
	// pthread_mutex_init (&mutex, &attr);

	if ( pthread_mutex_init(&mutex, &attr) != 0 )
	{
		throw  SimpleException("Error Mutex Init");
	}
#endif
#ifdef DEBUG
	OwnerId = 0;
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
	pthread_mutex_destroy(&mutex);
#endif
}

bool ReentrantMutex::Lock()
{
#ifdef WIN32
	unsigned int Result = WaitForSingleObject( mutex, INFINITE );
	if ( Result == WAIT_OBJECT_0 )
	{
#ifdef DEBUG
		// In debug mode, set the owner id
		OwnerId = Thread::GetThreadId();
#endif
		// Here we go, we've got the mutex
		return true;
	}
#else
	if( pthread_mutex_lock(&mutex) == 0 )
	{
#ifdef DEBUG
		// In debug mode, set the owner id
		OwnerId = GetThreadId();
#endif
		return true;
	}
#endif

	return false;
}

bool ReentrantMutex::Unlock()
{
#ifdef WIN32
	if ( !ReleaseMutex(mutex) )
	{
		return false;
	}
#else
	if( pthread_mutex_unlock(&mutex) != 0 )
	{
		return false;
	}
#endif
#ifdef DEBUG
	OwnerId = 0;
#endif

	return true;
}
