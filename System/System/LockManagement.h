/**
 * @file LockManagement.h
 * @ingroup System
 * @brief Declaration of LockableObject and SmartLock
 */

#ifndef __LOCK_MANAGEMENT_H__
#define __LOCK_MANAGEMENT_H__

#include <System/ConfigSystem.h>

namespace Omiscid {

/**
 * @class LockableObject LockManagement.h System/LockManagement.h
 * @brief LockableObject are object that can be lock (Mutex, ReentrantMutex, MutexedSimpleList, ...).
 *
 * @author Dominique Vaufreydaz
 */
class LockableObject
{
public:
	/** @brief Constructor
	 *
	 */
	LockableObject() {};

	/** @brief Constructor
	 *
	 */
	virtual ~LockableObject() {};

	/** @brief Virtual function used to lock the object
	 *
	 */
	virtual bool Lock() = 0;

	/** @brief Virtual function used to unlock the object
	 *
	 */
	virtual bool Unlock() = 0;
};

/**
 * @class SmartLocker LockManagement.h System/LockManagement.h
 * @brief SmartLocker can check the lock is removed and them
 * unlock it when destroyed. This class is designed to operate
 * in functions. It is not thread safe.
 *
 * @author Dominique Vaufreydaz
 */
class SmartLocker
{
public:
	/** @brief Constructor
	 * @param LockableObjectToManage a reference to the object to managed
	 * @param LockAtInit do we lock this LockAtInit
	 *
	 */
	SmartLocker( LockableObject& LockableObjectToManage, bool LockAtInit = true );

	/** @brief Constructor
	 * @param LockableObjectToManage a reference to the object to managed
	 *
	 */
	SmartLocker( const LockableObject& LockableObjectToManage, bool LockAtInit = true  );

	/** @brief Constructor
	 *
	 */
	virtual ~SmartLocker();

	/** @brief Function used to lock the LockableObject object
	 *
	 */
	bool Lock();

	/** @brief Function used to unlock the LockableObject object
	 *
	 */
	bool Unlock();

private:
	unsigned int LockCount;	/*<! Number of lock done on the LockableObject */

	LockableObject& ManagedLoackableObject;
};

} // namespace Omiscid

#endif // __LOCK_MANAGEMENT_H__

