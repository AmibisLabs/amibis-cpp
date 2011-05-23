/**
 * @file System/Mutex.h
 * @ingroup System
 * @brief Definition of Mutex class
 */

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <System/ConfigSystem.h>
#include <System/LockManagement.h>

namespace Omiscid {

/**
 * @class Mutex Mutex.cpp System/Mutex.h
 * @brief Mutex Implementation
 *
 * Mutex enables to lock some code section to avoid multiple access by several threads.
 * The mutex are not reentrant.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class Mutex : public LockableObject
{
public:
	/** @brief Constructor */
	Mutex();
	/** @brief Destructor */
	virtual ~Mutex();

	/**
	 * @brief Lock the mutex.
	 *
	 * Wait if the mutex is already locked, until it is unlocked, and then locks the mutex
	 * @return false if an error occured
	 */
	bool Lock();

	/**
	 * @brief Lock the mutex. Deprecated, use Mutex#Lock instead.
	 *
	 * Wait if the mutex is already locked, until it is unlocked, and then locks the mutex
	 * @return false if an error occured
	 */
	inline bool EnterMutex() { return Lock(); };

	/**
	 * @brief Unlock the mutex
	 *
	 * Enables other clients to use the critical section protected by this mutex.
	 */
	bool Unlock();

	/**
	 * @brief Unlock the mutex. Deprecated, use Mutex#Unlock instead.
	 *
	 * Enables other clients to use the critical section protected by this mutex.
	 */
	inline bool LeaveMutex() { return Unlock(); };

private:

#ifdef DEBUG
	unsigned int OwnerId;
	unsigned int PreviousOwnerId;
#endif

#ifdef WIN32
	HANDLE mutex;
#else
	unsigned int before[5];	/*!< to prevent memory correption by pthread_* functions */
	pthread_mutex_t mutex; /*!< Posix Mutex*/
	unsigned int after[5];		/*!< to prevent memory correption by pthread_* functions */
#endif /* WIN32 */

};

} // namespace Omiscid

#endif // __MUTEX_H__
