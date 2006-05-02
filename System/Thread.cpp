// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////


#include <System/Thread.h>
#include <System/Portage.h>
#include <stdio.h>

#ifndef WIN32
	#include <unistd.h>
	#include <sys/time.h>
#else
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#endif

#include <errno.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Thread::Thread(bool autostart)
{
#ifdef WIN32
	m_ThreadID = 0;
	m_pThread  = NULL;
	
	Event event;
#else
	m_isRunning = false;	
	m_stopRequired = false;
	
	if(pthread_mutex_init(&mutex, NULL) != 0)
		throw "Error Mutex Init";
	if(pthread_cond_init(&condition, NULL) != 0)
		throw "Error Condition Init";
#endif
	if (autostart) StartThread();
}

Thread::~Thread()
{
#ifdef WIN32
	int timeout=100;
	if ( m_pThread )
	{
		if (m_ThreadID>1)
			StopThread();
		else
		{
			while ((timeout--) && (m_ThreadID==1))
				Sleep(10);
			if (!timeout)
			{
				TraceError( "<Thread>  Destructor has timed out while waiting for thread to exit.\r\n");
			}
		}
		CloseHandle( m_pThread );
		m_pThread  = 0;	
		m_ThreadID = 0;	
	}
#else
	if(Running())
	{
		StopThread(1000);
	}
#endif
}

bool Thread::StartThread()
{
	// restart thread if already running
	if (Running()) StopThread();

#ifdef WIN32
	m_ThreadID = 0;
	m_pThread = CreateThread( NULL, 0, s_run, (LPVOID)this, 0, &m_ThreadID );
	return (m_pThread != NULL);
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int rc = pthread_create(&m_thread, &attr, CallRun, this);
	pthread_attr_destroy(&attr);
	return rc == 0;
#endif
}

bool Thread::StopThread(int wait_ms )
{
#ifdef WIN32	
	if (Running())
	{
		m_ThreadID = 1;

		event.Wait(wait_ms);
	}
	
#else
	pthread_mutex_lock(&mutex);
	if (Running())
	{
		m_stopRequired = true;
		
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
unsigned long __stdcall Thread::s_run(LPVOID importobj)
{
	((Thread*)importobj)->event.Reset();

	((Thread*)importobj)->Run();
	((Thread*)importobj)->m_ThreadID = 0;
	((Thread*)importobj)->m_pThread = 0;
	
	((Thread*)importobj)->event.Signal();
	
	return 0;
}
#else
void* Thread::CallRun(void* ptr)
{
	Thread* t = (Thread*)ptr;
	t->m_isRunning = true;
	t->Run();
	pthread_mutex_lock(&(t->mutex));
	t->m_isRunning = false;
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
