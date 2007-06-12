/**
 * @file ReentrantMutex.h
 * @ingroup System
 * @brief Definition of ReentrantMutex class
 */

#ifndef __REENTRANT_MUTEX_H__
#define __REENTRANT_MUTEX_H__

#include <System/Config.h>

namespace Omiscid {

/**
 * @class ReentrantMutex ReentrantMutex.h System/ReentrantMutex.h
 * @brief ReentrantMutex Implementation
 *
 * ReentrantMutex enables to lock some code section to avoid multiple access by several threads.
 * These mutex are reentrant.
 * @author Dominique Vaufreydaz
 */

class ReentrantMutex
{
public:
	/** @brief Constructor */
	ReentrantMutex();

	/** @brief Destructor */
	virtual ~ReentrantMutex();

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
	#ifdef DEBUG
		DWORD OwnerId;
	#endif
#else
	pthread_mutex_t mutex; /*!< Posix Mutex*/
#endif /* WIN32 */
};

} // namespace Omiscid

#endif // __REENTRANT_MUTEX_H__
