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

Thread::Thread(bool autostart)
{
#ifdef WIN32
	ThreadID = 0;
	ThreadHandle  = NULL;
	IsRunning = false;	
	StopWasAsked = false;
	
	// Event event;
#else
	IsRunning = false;	
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
	if(Running())
	{
		StopThread(DEFAULT_THREAD_DESTRUCTOR_TIMEOUT);
	}
}

bool Thread::StartThread()
{
	// do nothing if already running
	if (Running())
		return false;

	IsRunning = false;	
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
	if (Running())
	{
		StopWasAsked = true;
		if ( event.Wait(wait_ms) == false )
		{
			// Timeout !!!
			TraceError( "Thread::StopThread: Thread %u do not stop before timeout (%d)\n", ThreadID, wait_ms );
		}

		// Close the Thread handle
		CloseHandle( ThreadHandle );

		ThreadID = 0;
		ThreadHandle = NULL;
		IsRunning = false;	
		StopWasAsked = false;

	}
#else
	pthread_mutex_lock(&mutex);
	if (Running())
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
unsigned long __stdcall Thread::CallRun(void* ptr)
{
	Thread* t = (Thread*)ptr;

	// init thread random generator
	RandomInit();

	// Reset stat event
	t->event.Reset();

	// Do my job
	t->Run();

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

	t->IsRunning = true;
	t->Run();
	pthread_mutex_lock(&(t->mutex));
	t->IsRunning = false;
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

bool Thread::Running() const 
{ 
#ifdef WIN32
	return (ThreadHandle != 0); 
#else
	return IsRunning;
#endif
}

bool Thread::StopPending() const 
{ 
	return StopWasAsked;
}
