/**
 * @file Event.h
 * @brief Definition of Event class
 */
#ifndef _EVENT_H_
#define _EVENT_H_

#ifndef WIN32
	#include <pthread.h>
#else
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#endif /* WIN32 */

/** 
 * @class Event Event.h System/Event.h
 * @brief Event or condition implementation.
 *
 * Enable a thread to wait on a condition, until be awoken by another thread.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class Event
{
public:
	/** @brief Constructor */
	Event();
	/** @brief Destructor */
	~Event();
	
	/** @brief Signal all the threads waiting on this event*/
	void Signal();
	
	/** @brief Reset the event
	 *
	 * (use under Windows : stop the effect of the method 'Signal'.)
	 */
	void Reset(); 
	
	/** @brief Block the thread
	 * 
	 * Block the thread on this event until another thread signal the event, 
	 * during a maximum of 'timer' milliseconds (if 'timer' no null)
	 * @param [in] timer timeout in milliseconds. 0 to wait an infinite time.
	 */
	bool Wait(unsigned long timer = 0);
private:
#ifdef WIN32
	HANDLE handle;
#else
	pthread_cond_t condition; /*!< the condition object */
	pthread_mutex_t mutex; /*!< mutex to protect the access to the condition */
#endif
};

#endif /* _EVENT_H_ */
