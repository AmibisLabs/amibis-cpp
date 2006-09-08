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
Thread::Thread(bool autostart, const SimpleString Name /* = SimpleString::EmptyString */)
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
	bool ThreadStopInTime = true;

	if (IsRunning())
	{
		// Notify thread !
		StopWasAsked = true;			// Notification for pseudo active waiting
		StopWasAskedEvent.Signal();		// other notification

		ThreadStopInTime = IsEnded.Wait(wait_ms);
		if ( ThreadStopInTime == false )
		{
			// Timeout !!!
			TraceError( "Thread::StopThread: Thread %u do not stop before timeout (%d).\n", ThreadID, wait_ms );
#ifdef WIN32
			// TerminateThread( ThreadHandle, 0 );
#endif
		}

//		Locker.EnterMutex();

#ifdef WIN32

		// Close the Thread handle
		// CloseHandle( ThreadHandle );

		ThreadID = 0;
		ThreadHandle = NULL;
#else
		// m_thread	: something to do with ?
#endif

//		Locker.LeaveMutex();
	}

	return ThreadStopInTime;
}

#ifdef WIN32
unsigned long FUNCTION_CALL_TYPE Thread::CallRun(void* ptr)
#else
void Thread::CallRun(void* ptr)
#endif
{
	Thread* t = (Thread*)ptr;

	// init thread random generator
	RandomInit();

	// Reset stat event
	t->IsEnded.Reset();

#ifdef DEBUG
	// Trace( "%s\n", t->ThreadName.GetStr() );
#endif

	// Do my job
	t->ThreadIsRunning = true;
	t->Run();
	t->ThreadIsRunning = false;

	// revert my data
	t->ThreadID = 0;
	t->ThreadHandle = 0;
	
	// signal, my job is over
	t->IsEnded.Signal();

#ifdef WIN32
	return 0;
#else
	pthread_exit(NULL);
#endif
}

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
	return ThreadIsRunning;
}

bool Thread::StopPending() const 
{ 
	return StopWasAsked;
}

void Thread::WaitForStop()
{
	StopWasAskedEvent.Wait(0);
}
