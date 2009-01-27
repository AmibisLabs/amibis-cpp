/**
* @file System/Thread.cpp
* @ingroup System
* @brief Implementation of ThreadMessage and Thread classes
*/


#include <System/Thread.h>
#include <System/Portage.h>
#include <System/ElapsedTime.h>

#include <errno.h>

using namespace Omiscid;

	/** @brief Constructor
	 *
	 */
ThreadMessage::ThreadMessage()
{
	Code   = 0;
	Param1 = (void*)NULL;
	Param2 = (void*)NULL;
}

	/** @brief Constructor
	 *
	 */
ThreadMessage::ThreadMessage(int cCode, void * cParam1, void* cParam2 /* = (void*)NULL */ )
{
	Code   = cCode;
	Param1 = cParam1;
	Param2 = cParam2;
}

/** @brief Constructor
	 *
	 */
ThreadMessage::~ThreadMessage()
{
}

const ThreadMessage& ThreadMessage::operator=(const ThreadMessage& ToCopy)
{
	Code   = ToCopy.Code;
	Param1 = ToCopy.Param1;
	Param2 = ToCopy.Param2;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef DEBUG_THREAD
Thread::Thread(const SimpleString Name /* = SimpleString::EmptyString */)
#else
Thread::Thread()
#endif
{
#ifdef DEBUG_THREAD
	ThreadName = Name;
#endif

#ifdef WIN32
	ThreadID = 0;
	ThreadHandle  = NULL;
#else
	m_thread = (pthread_t)0;
#endif

	ThreadIsRunning = false;
	StopWasAsked = false;
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
	if ( IsRunning() )
	{
		return false;
	}

	ThreadIsRunning = false;
	StopWasAsked = false;

#ifdef DEBUG_THREAD
	OmiscidTrace( "StartThread : %s\n", ThreadName.GetStr() );
#endif

#ifdef WIN32
	ThreadID = 0;
	ThreadHandle = CreateThread( NULL, 0, CallRun, (void*)this, 0, &ThreadID );
	return (ThreadHandle != NULL);
#else
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
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
#ifdef DEBUG_THREAD
		OmiscidTrace( "Ask %s to stop.\n", ThreadName.GetStr() );
#endif

		// Notify thread !
		StopWasAsked = true;			// Notification for pseudo active waiting
		StopWasAskedEvent.Signal();		// other notification

		ThreadStopInTime = IsEnded.Wait(wait_ms);
		if ( ThreadStopInTime == false )
		{
			// Timeout !!!
#ifdef DEBUG_THREAD
			OmiscidError( "Thread::StopThread: %s do not stop before timeout (%d).\n",  ThreadName.GetStr(), wait_ms );
#else
			OmiscidError( "Thread::StopThread: Thread do not stop before timeout (%d).\n",  wait_ms );
#endif

			// Destroy the thread...
#ifdef WIN32
			// TerminateThread( ThreadHandle, 0 );
			ThreadID = 0;
			ThreadHandle  = NULL;
#else
			// pthread_cancel(m_thread);
#endif
		}
	}

#ifdef DEBUG_THREAD
	OmiscidTrace( "Thread %s stopped.\n", ThreadName.GetStr() );
#endif

	return ThreadStopInTime;
}

#ifdef WIN32
unsigned long FUNCTION_CALL_TYPE Thread::CallRun(void* ptr)
#else
void* Thread::CallRun(void* ptr)
#endif
{
	Thread* t = (Thread*)ptr;

	// init thread random generator
	RandomInit();

	// Reset stat event
	t->IsEnded.Reset();

#ifdef DEBUG_THREAD
	t->ThreadName += "(";
	t->ThreadName += t->GetThreadId();
	t->ThreadName += ")";
	OmiscidTrace( "%s started\n", t->ThreadName.GetStr() );
#endif

	// Do my job
	t->ThreadIsRunning = true;
	t->Run();
	t->ThreadIsRunning = false;

#ifdef WIN32

	// Close the Thread handle
	// CloseHandle( ThreadHandle );

	t->ThreadID = 0;
	t->ThreadHandle = NULL;
#else
	// m_thread	: something to do with ?
#endif

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
	// Use SleepEx because accuracy of the wait time is better than with ::Sleep
	::SleepEx(nb_ms, FALSE);
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

/** @brief return an Id for the calling Thread
 *
 */
unsigned int Thread::GetThreadId()
{
#ifdef WIN32
	return (unsigned int)GetCurrentThreadId();
#else
	return (unsigned int)pthread_self();
#endif
}

	/** @brief Send message to a Thread
	 *
	 * Send a message to a thread
	 */
void Thread::SendMessage( int Code, void * Param1, void* Param2 /* = (void*)NULL */ )
{
	ThreadMessage MsgToSend( Code, Param1, Param2 );

	SmartLocker SL_MsgQueue(MsgQueue);

	MsgQueue.AddTail(MsgToSend);
}

	/** @brief Send message to a Thread
	 *
	 * Send a message to a thread
	 */
void Thread::SendMessage( const ThreadMessage& MsgToSend )
{
	SmartLocker SL_MsgQueue(MsgQueue);

	MsgQueue.AddTail(MsgToSend);
}

	/** @brief Retrieve message for a thread with timeout
	 *
	 * Retrieve message for a thread
	 */
bool Thread::WaitAndGetMessage( ThreadMessage& MsgToGet, unsigned int DelayMax /* = (unsigned int)DEFAULT_MESSAGE_TIMEOUT */ )
{
	bool Done;

	SmartLocker SL_MsgQueue(MsgQueue, false);

	if ( DelayMax == 0 )
	{
		// INFINITE wait
		Done = false;
		while( true ) // was for(;;) but new version of MS compiler do no like it
		{
			// Is there a message ?
			SL_MsgQueue.Lock();
			if ( MsgQueue.GetNumberOfElements() != 0 )
			{
				MsgToGet = MsgQueue.ExtractFirst();
				Done = true;
			}
			SL_MsgQueue.Unlock();

			if ( Done == true )
			{
				return true;
			}

			// Wait
			Sleep( 10 );
		}
		// never here
		return true;
	}
	else
	{
		ElapsedTime CountWaitedTime;
		Done = false;
		while( true ) // was for(;;) but new version of MS compiler do no like it
		{
			// Is there a message ?
			SL_MsgQueue.Lock();
			if ( MsgQueue.GetNumberOfElements() != 0 )
			{
				MsgToGet = MsgQueue.ExtractFirst();
				Done = true;
			}
			SL_MsgQueue.Unlock();

			if ( Done == true )
			{
				return true;
			}

			if ( Done == true || CountWaitedTime.Get() >= DelayMax )
			{
				return Done;
			}

			Sleep( 10 );
		}
		// never here
		return true;
	}

	return false;
}

	/** @brief Get a message for a thread
	 *
	 * Get message for a thread
	 */
bool Thread::GetMessage( ThreadMessage& MsgToSend )
{
	SmartLocker SL_MsgQueue(MsgQueue);

	if ( MsgQueue.GetNumberOfElements() == 0 )
	{
		// No message
		return false;
	}

	// get first message
	MsgToSend = MsgQueue.ExtractFirst();
	return true;
}
