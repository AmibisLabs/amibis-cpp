// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////


#include <System/Thread.h>
#include <System/Portage.h>

#include <errno.h>

using namespace Omiscid;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef DEBUG
Thread::Thread(const SimpleString Name, bool autostart)
#else
Thread::Thread(bool autostart)
#endif
{
#ifdef DEBUG
	ThreadName = Name;
#endif


#ifdef WIN32
	ThreadID = 0;
	ThreadHandle  = NULL;
	ThreadIsRunning = false;	
	StopWasAsked = false;
	
	// Event event;
#else
	ThreadIsRunning = false;	
	StopWasAsked = false;
	
	if(pthread_mutex_init(&mutex, NULL) != 0)
		throw "Error Mutex Init";
	if(pthread_cond_init(&condition, NULL) != 0)
		throw "Error Condition Init";
#endif
	
	if (autostart)
		StartThread();
}

Thread::~Thread()
{
	if(IsRunning())
	{
		StopThread();
	}
}

bool Thread::StartThread()
{
	// do nothing if already running
	if (IsRunning())
		return false;

	ThreadIsRunning = false;	
	StopWasAsked = false;

#ifdef WIN32
	ThreadID = 0;
	ThreadHandle = CreateThread( NULL, 0, CallRun, (void*)this, 0, &ThreadID );
	return (ThreadHandle != NULL);
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int rc = pthread_create(&m_thread, &attr, CallRun, this);
	pthread_attr_destroy(&attr);
	return (rc == 0);
#endif
}

bool Thread::StopThread(int wait_ms)
{
#ifdef WIN32	
	if (IsRunning())
	{
		StopWasAsked = true;
		if ( event.Wait(wait_ms) == false )
		{
			// Timeout !!!
			TraceError( "Thread::StopThread: Thread %u do not stop before timeout (%d).\n", ThreadID, wait_ms );
			// TerminateThread( ThreadHandle, 0 );
		}

		// Close the Thread handle
		// CloseHandle( ThreadHandle );

		ThreadID = 0;
		ThreadHandle = NULL;
		ThreadIsRunning = false;	
		StopWasAsked = false;

	}
#else
	pthread_mutex_lock(&mutex);
	if (IsRunning())
	{
		StopWasAsked = true;
		
		if(wait_ms != 0)
		{
			struct timeval now;
			struct timespec timeout; 
			int retcode;
			int second = wait_ms/1000;
			int nanos = (wait_ms-second*1000)*1000000;
		
			gettimeofday(&now, NULL);
			timeout.tv_sec = now.tv_sec + second;
			timeout.tv_nsec = now.tv_usec * 1000 + nanos;
		
			retcode = pthread_cond_timedwait(&condition, &mutex, &timeout);
			pthread_mutex_unlock(&mutex);

			if ( retcode == ETIMEDOUT )
			{
				TraceError( "Thread::StopThread: Thread do not stop before timeout (%d).\n", wait_ms );
			}
			return (retcode != ETIMEDOUT);
		}
		else
		{
			pthread_cond_wait(&condition, &mutex);
			pthread_mutex_unlock(&mutex);
			return true;
		}
	}
	pthread_mutex_unlock(&mutex);
#endif
	return true;
}

#ifdef WIN32
unsigned long FUNCTION_CALL_TYPE Thread::CallRun(void* ptr)
{
	Thread* t = (Thread*)ptr;

	// init thread random generator
	RandomInit();

	// Reset stat event
	t->event.Reset();

#ifdef DEBUG
	// TraceError( "%s\n", t->ThreadName.GetStr() );
#endif

	// Do my job
	t->ThreadIsRunning = true;
	t->Run();
	t->ThreadIsRunning = false;

	// revert my data
	t->ThreadID = 0;
	t->ThreadHandle = 0;
	
	// signal, my job is over
	t->event.Signal();
	// TraceError( "ThreadSignaled\n" );
	
	return 0;
}
#else
void* Thread::CallRun(void* ptr)
{
	Thread* t = (Thread*)ptr;

	// init thread random generator
	RandomInit();

	t->ThreadIsRunning = true;

#ifdef DEBUG
	// TraceError( "%s\n", t->ThreadName.GetStr() );
#endif

	t->Run();
	pthread_mutex_lock(&(t->mutex));
	t->ThreadIsRunning = false;
	pthread_cond_broadcast(&(t->condition));	
	pthread_mutex_unlock(&(t->mutex));
	pthread_exit(NULL);
}
#endif /* WIN32 */


void Thread::Sleep(int nb_ms)
{
#ifdef WIN32
	::Sleep(nb_ms);
#else
	usleep(nb_ms*1000);	
#endif
}

bool Thread::IsRunning() const 
{ 
#ifdef WIN32
	return (ThreadHandle != 0); 
#else
	return ThreadIsRunning;
#endif
}

bool Thread::StopPending() const 
{ 
	return StopWasAsked;
}
