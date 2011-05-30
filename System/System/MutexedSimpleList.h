/**
* @file SimpleList.h
* @ingroup System
* @ingroup UserFriendly
* @brief Definition of SimpleListElement class,
* SimpleList class and MutexedSimpleList class.
*/

#ifndef __MUTEXED_SIMPLE_LIST_H__
#define __MUTEXED_SIMPLE_LIST_H__

#include <System/ConfigSystem.h>
#include <System/SimpleList.h>
#include <System/ReentrantMutex.h>

namespace Omiscid {

/**
* @class MutexedSimpleList SimpleList.h System/SimpleList.h
* @brief Combination of a simple list with a mutex.
*
* The mutex is used to lock and unlock the access to the simple list.
* When the user want to lock an access, he can call Lock and Unlock method.
* @author Dominique Vaufreydaz
*/
template <typename TYPE>
class MutexedSimpleList : public SimpleList<TYPE>, public LockableObject
{
public:
#ifdef DEBUG_MSL
	MutexedSimpleList()
	{
		NbLocks = 0; // MutexedSimpleList is not lock
	}

	virtual ~MutexedSimpleList()
	{
		NbLocks = 100; // MutexedSimpleList is not lock anymore,
		// All operation will be permitted to destroy the list
	}

	bool Add( const TYPE& Val )
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using Add on a non lock list.\n" );
		}
		return SimpleList<TYPE>::Add(Val);
	}

	bool AddHead( const TYPE& Val )
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using AddHeah on a non lock list.\n" );
		}
		return SimpleList<TYPE>::AddHead(Val);
	}

	bool AddTail( const TYPE& Val )
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using AddTail on a non lock list.\n" );
		}
		return SimpleList<TYPE>::AddTail(Val);
	}

	unsigned int GetNumberOfElements() const
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using GetNumberOfElements on a non lock list.\n" );
		}
		return SimpleList<TYPE>::GetNumberOfElements();
	}

	void First()
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using First on a non lock list.\n" );
		}
		SimpleList<TYPE>::First();
	}

	bool Next()
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using Next on a non lock list.\n" );
		}
		return SimpleList<TYPE>::Next();
	}

	bool AtEnd() const
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using AtEnd on a non lock list.\n" );
		}
		return SimpleList<TYPE>::AtEnd();
	}

	bool NotAtEnd() const
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using NotAtEnd on a non lock list.\n" );
		}
		return SimpleList<TYPE>::NotAtEnd();
	}

	TYPE& GetCurrent() const
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using GetCurrent on a non lock list.\n" );
		}
		return SimpleList<TYPE>::GetCurrent();
	}

	bool RemoveCurrent()
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using RemoveCurrent on a non lock list.\n" );
		}
		return SimpleList<TYPE>::RemoveCurrent();
	}

	bool IsEmpty() const
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using IsEmpty on a non lock list.\n" );
		}
		return SimpleList<TYPE>::IsEmpty();
	}

	bool IsNotEmpty() const
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using IsNotEmpty on a non lock list.\n" );
		}
		return SimpleList<TYPE>::IsNotEmpty();
	}

	TYPE ExtractFirst()
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using ExtractFirst on a non lock list.\n" );
		}
		return SimpleList<TYPE>::ExtractFirst();
	}

	bool Remove(const TYPE& Element)
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using Remove on a non lock list.\n" );
		}
		return SimpleList<TYPE>::Remove(Element);
	}

	virtual void Empty()
	{
		if ( NbLocks == 0 )
		{
			fprintf( stderr, "Using Empty on a non lock list.\n" );
		}
		SimpleList<TYPE>::Empty();
	}

private:
	AtomicReentrantCounter NbLocks;
#else
public:
	virtual ~MutexedSimpleList()
	{
	}
#endif

public:
	   /** @brief equal operator
	*
	* Build a copy of the list.
	*/
	MutexedSimpleList<TYPE>& operator=(MutexedSimpleList<TYPE>& ToCopy)
	{
		SmartLocker SL_ToCopy(ToCopy);
		SmartLocker SL_Myself(*this);

		SimpleList<TYPE>::Init(); // SimpleList<TYPE>:: to make gcc happy

		// We can not call SimpleList<TYPE>::operator= because of instrumented version
		for( ToCopy.First(); ToCopy.NotAtEnd(); ToCopy.Next() )
		{
			// Add tail in order to preserve the order of the list ToCopy
			AddTail(ToCopy.GetCurrent());
		}

		return *this;
	}

	/** @brief equal operator
	*
	* Build a copy of the list.
	*/
	MutexedSimpleList<TYPE>& operator=(SimpleList<TYPE>& ToCopy)
	{
		SmartLocker SL_Myself(*this);

		SimpleList<TYPE>::Init(); // SimpleList<TYPE>:: to make gcc happy

		// We can not call SimpleList<TYPE>::operator= because of instrumented version
		for( ToCopy.First(); ToCopy.NotAtEnd(); ToCopy.Next() )
		{
			// Add tail in order to preserve the order of the list ToCopy
			AddTail(ToCopy.GetCurrent());
		}

		return *this;
	}

	/** @brief Lock the access to the list
	*
	* Wait until the mutex can be locked.
	* @return if the 'lock' on the mutex is successful
	*/
	bool Lock();

	/** @brief Unlock the access to the list
	*
	* Enable another thread to lock the list.
	* @return if the 'unlock' on the mutex is successful
	*/
	bool Unlock();

private:
	ReentrantMutex mutex; /*!< the mutex to protect access to the list*/
};

template <typename TYPE>
bool MutexedSimpleList<TYPE>::Lock()
{
#ifdef DEBUG_MSL
	// Only for MutexedSimpleList debugging
	bool ret = mutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	if ( ret == true )
	{
		NbLocks++;
	}
	return ret;
#else
	return mutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
#endif
}

template <typename TYPE>
bool MutexedSimpleList<TYPE>::Unlock()
{
#ifdef DEBUG_MSL
	if ( NbLocks == 0 )
	{
		OmiscidTrace( "List already unlock. something goes wrong ?" );
	}

	// Only for MutexedSimpleList debugging
	bool ret = mutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	if ( ret == true && NbLocks > 0)
	{
		NbLocks++;
	}
	return ret;
#else
	return mutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
#endif
}

} // namespace Omiscid

#endif // __MUTEXED_SIMPLE_LIST_H__

