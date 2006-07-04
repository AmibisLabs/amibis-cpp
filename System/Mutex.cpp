// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////


#include <System/Mutex.h>

using namespace Omiscid;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Mutex::Mutex()
{
#ifdef WIN32
	// mutex = CreateMutex( NULL, false, NULL );
	mutex = CreateSemaphore(NULL, 1, 2^32-1, NULL );
	OwnerId = 0;
#else
	if(pthread_mutex_init(&mutex, NULL) != 0)
		throw "Error Mutex Init";
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
	if(pthread_mutex_destroy(&mutex) != 0)
		throw "Error Mutex Destroy";
#endif
}

bool Mutex::EnterMutex()
{
#ifdef WIN32
	unsigned int Result = WaitForSingleObject( mutex, INFINITE );
	if ( Result == WAIT_OBJECT_0 )
	{
		// Here we go, we've got the mutex
		OwnerId = GetCurrentThreadId();
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
	OwnerId = 0;

#else
	if( pthread_mutex_unlock(&mutex) != 0 )
	{
		return false;
	}
#endif

	return true;
}

ReentrantMutex::ReentrantMutex()
{
#ifdef WIN32
	mutex = CreateMutex(NULL, false, NULL );
	OwnerId = 0;
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init (&mutex, &attr);

	if(pthread_mutex_init(&mutex, &attr) != 0)
		throw "Error Mutex Init";
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
	if(pthread_mutex_destroy(&mutex) != 0)
		throw "Error Mutex Destroy";
#endif
}

bool ReentrantMutex::EnterMutex()
{
#ifdef WIN32
	unsigned int Result = WaitForSingleObject( mutex, INFINITE );
	if ( Result == WAIT_OBJECT_0 )
	{
		// Here we go, we've got the mutex
		OwnerId = GetCurrentThreadId();
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
	OwnerId = 0;

#else
	if( pthread_mutex_unlock(&mutex) != 0 )
	{
		return false;
	}
#endif

	return true;
}
