#include <System/Event.h>

#include <errno.h>

using namespace Omiscid;

Event::Event()
{
#ifndef WIN32
	if(pthread_mutex_init(&mutex, NULL) != 0)
		throw "Error Mutex Init";
	if(pthread_cond_init(&condition, NULL) != 0)
		throw "Error Condition Init";
#else
	if((handle = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		throw "Error CreateEvent";
#endif /* WIN32 */
}

Event::~Event()
{
#ifndef WIN32
	pthread_cond_destroy(&condition);
	pthread_mutex_destroy(&mutex);
#else
	CloseHandle(handle);
#endif /* WIN32 */
}

void Event::Reset()
{
#ifdef WIN32
	ResetEvent(handle);
#endif
}

void Event::Signal()
{
#ifndef WIN32
	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&condition);
	pthread_mutex_unlock(&mutex);
#else
	SetEvent(handle);
#endif /* WIN32 */
}

bool Event::Wait(unsigned long timer)
{
#ifndef WIN32
	pthread_mutex_lock(&mutex);
	if(timer == 0)
	{
		pthread_cond_wait(&condition, &mutex);
		pthread_mutex_unlock(&mutex);
		return true;
	}
	else
	{
		struct timeval now;
		struct timespec timeout;
		int retcode;

		int second = timer/1000;
		int nanos = (timer-second*1000)*1000000;
		
		gettimeofday(&now, NULL);
		timeout.tv_sec = now.tv_sec + second;
		timeout.tv_nsec = now.tv_usec * 1000 + nanos;
		
		retcode = pthread_cond_timedwait(&condition, &mutex, &timeout);
		pthread_mutex_unlock(&mutex);
		return (retcode != ETIMEDOUT);
	}
#else
	if ( timer == 0 )
	{
		for(;;)
		{
			int ret = WaitForSingleObject(handle, INFINITE);
			switch( ret )
			{
				case WAIT_TIMEOUT:
					continue;

				case WAIT_OBJECT_0:
					return true;

				default:
					return false;
			}
		}
	}
	else
	{
		if ( WaitForSingleObject(handle, timer) == WAIT_OBJECT_0 )
		{
			// We've got it !
			return true;
		}
		return false;
	}
#endif
}
