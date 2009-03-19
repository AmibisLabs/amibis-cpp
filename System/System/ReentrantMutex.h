/**
 * @file ReentrantMutex.h
 * @ingroup System
 * @brief Definition of ReentrantMutex class
 */

#ifndef __REENTRANT_MUTEX_H__
#define __REENTRANT_MUTEX_H__

#include <System/ConfigSystem.h>
#include <System/LockManagement.h>

namespace Omiscid {

template <typename TYPE> class SimpleList;

/**
 * @class ReentrantMutex ReentrantMutex.h System/ReentrantMutex.h
 * @brief ReentrantMutex Implementation
 *
 * ReentrantMutex enables to lock some code section to avoid multiple access by several threads.
 * These mutex are reentrant.
 * @author Dominique Vaufreydaz
 */

class ReentrantMutex : public LockableObject
{
public:
	/** @brief Constructor */
	ReentrantMutex();

	/** @brief Destructor */
	virtual ~ReentrantMutex();

	/**
	 * @brief Lock the mutex. Deprecated, use ReentrantMutex#Lock instead
	 *
	 * Wait if the mutex is already locked, until it is unlocked, and then locks the mutex
	 * @return false if an error occured
	 */
	bool Lock();

	/**
	 * @brief Lock the mutex. Deprecated, use ReentrantMutex#Lock instead
	 *
	 * Wait if the mutex is already locked, until it is unlocked, and then locks the mutex
	 * @return false if an error occured
	 */
	inline bool EnterMutex() { return Lock(); };

	/**
	 * @brief Unlock the mutex. Deprecated, use ReentrantMutex#Unlock instead
	 *
	 * Enables other clients to use the critical section protected by this mutex.
	 */
	bool Unlock();

	/**
	 * @brief Unlock the mutex. Deprecated, use ReentrantMutex#Unlock instead
	 *
	 * Enables other clients to use the critical section protected by this mutex.
	 */
	inline bool LeaveMutex() { return Unlock(); };

private:

#ifdef DEBUG
	SimpleList<unsigned int> * OwnerIds;
	unsigned int PreviousOwnerId;
#endif

#ifdef WIN32
	HANDLE mutex;
#else
	unsigned int before1;	/*!< to prevent memory correption by pthread_* functions */
	unsigned int before2;	/*!< to prevent memory correption by pthread_* functions */
	unsigned int before3;	/*!< to prevent memory correption by pthread_* functions */
	unsigned int before4;	/*!< to prevent memory correption by pthread_* functions */
	unsigned int before5;	/*!< to prevent memory correption by pthread_* functions */
	pthread_mutex_t mutex; /*!< Posix Mutex */
	unsigned int after1;		/*!< to prevent memory correption by pthread_* functions */
	unsigned int after2;		/*!< to prevent memory correption by pthread_* functions */
	unsigned int after3;		/*!< to prevent memory correption by pthread_* functions */
	unsigned int after4;		/*!< to prevent memory correption by pthread_* functions */
	unsigned int after5;		/*!< to prevent memory correption by pthread_* functions */
#endif /* WIN32 */
};

} // namespace Omiscid

#endif // __REENTRANT_MUTEX_H__
