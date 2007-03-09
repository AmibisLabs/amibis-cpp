/**
 * @file System/AutoDelete.h
 * @ingroup System
 * @brief Definition of a template class for autodeleting pointers when leaving scope
 */
#ifndef __AUTO_DELETE_H__
#define __AUTO_DELETE_H__

#include <System/Config.h>

#include <System/SimpleException.h>

namespace Omiscid {

/** 
 * @class Event System/AutoDelete.h
 * @brief Event or condition implementation.
 *
 * Enable a thread to wait on a condition, until be awoken by another thread.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
template <typename TYPE>
class AutoDelete
{
public:
	/** @brief Constructor */
	AutoDelete();

	/** @brief Copy constructor */
	AutoDelete(TYPE * ToCopy);

	/** @brief Copy constructor not allowed */
	AutoDelete(const AutoDelete<TYPE>& ToCopy);

	/** @brief Destructor */
	virtual ~AutoDelete();

	/** @brief Explicitally destroy the pointed object */
	void Delete(); 
	
	/** @brief Access function true '*' operator */
	TYPE& operator*();

	/** @brief Access function true '->' operator */
    TYPE* operator->();

private:
	TYPE * pObject;
};

/** @brief Constructor */
template <typename TYPE>
AutoDelete<TYPE>::AutoDelete()
{
	pObject = (TYPE*)NULL;
}

/** @brief Copy constructor */
template <typename TYPE>
AutoDelete<TYPE>::AutoDelete(TYPE * ToCopy)
{
	pObject = ToCopy;
}

/** @brief Copy constructor not allowed */
template <typename TYPE>
AutoDelete<TYPE>::AutoDelete(const AutoDelete<TYPE>& ToCopy)
{
	throw SimpleException( "Copying AutoDelete object will result in multiple delete calls on the same pointer", 0 );
}


/** @brief Destructor */
template <typename TYPE>
AutoDelete<TYPE>::~AutoDelete()
{
	Delete();
}
	
/** @brief Explicitally destroy the pointed object */
template <typename TYPE>
void AutoDelete<TYPE>::Delete()
{
	if ( pObject != NULL )
	{
		delete pObject;
		pObject = (TYPE*)NULL;
	}
}
	
/** @brief Access function true '*' operator */
template <typename TYPE>
TYPE& AutoDelete<TYPE>::operator*()
{
	return *pObject;
}

/** @brief Access function true '->' operator */
template <typename TYPE>
TYPE* AutoDelete<TYPE>::operator->()
{
	return pObject;
}

} // namespace Omiscid

#endif // __AUTO_DELETE_H__
