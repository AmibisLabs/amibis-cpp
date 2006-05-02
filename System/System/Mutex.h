/**
 * @file Mutex.h
 * @brief Definition of Mutex class
 */

#ifndef _MUTEX_H_
#define _MUTEX_H_

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
 * @class Mutex Mutex.h System/Mutex.h
 * @brief Mutex Implementation
 *
 * Mutex enables to lock some code section to avoid multiple access by several threads.
 * The mutex are not reentrant.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class Mutex
{
public:
	/** @brief Constructor */
	Mutex();
	/** @brief Destructor */
	~Mutex();

	/**
	 * @brief Lock the mutex. 
	 *
	 * Wait if the mutex is already locked, until it is unlocked, and then locks the mutex
	 * @return false if an error occured
	 */
	bool EnterMutex();
	
	/**
	 * @brief Unlock the mutex
	 *
	 * Enables other clients to use the critical section protected by this mutex.
	 */
	bool LeaveMutex();
	
private:
#ifdef WIN32
	HANDLE mutex;
	DWORD OwnerId;
#else
	pthread_mutex_t mutex; /*!< Posix Mutex*/
#endif /* WIN32 */
};

#endif /* _MUTEX_H_ */
