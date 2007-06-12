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

	/** @brief affectation operator/copy */
	const AutoDelete<TYPE>& operator=(const AutoDelete<TYPE>& ToCopy);

	/** @brief affectation operator/copy with content type */
	const AutoDelete<TYPE>& operator=(TYPE * ToCopy);

	/** @brief comparaison operator */
	bool operator==(const TYPE * ToCompare);

	/** @brief comparaison operator */
	bool operator==(const AutoDelete<TYPE>& ToCompare);

	/** @brief comparaison operator */
	bool operator!=(const TYPE * ToCompare);

	/** @brief comparaison operator */
	bool operator!=(const AutoDelete<TYPE>& ToCompare);

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
	if ( pObject != (TYPE*)NULL )
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

template <typename TYPE>
const AutoDelete<TYPE>& AutoDelete<TYPE>::operator=(const AutoDelete<TYPE>& ToCopy)
{
	throw SimpleException( "Copying AutoDelete object will result in multiple delete calls on the same pointer", 0 );
	return *this;
}

template <typename TYPE>
const AutoDelete<TYPE>& AutoDelete<TYPE>::operator=(TYPE * ToCopy)
{
	if ( pObject != (TYPE*)NULL )
	{
		throw SimpleException( "Copying AutoDelete object will result in multiple delete calls on the same pointer", 0 );
	}

	// Do affectation
	pObject = ToCopy;

	return *this;
}

/** @brief comparaison operator */
template <typename TYPE>
bool AutoDelete<TYPE>::operator==(const TYPE * ToCompare)
{
	// Are internal pointer equals to the given one ?
	if ( pObject == ToCompare )
	{
		return true;
	}
	return false;
}

/** @brief comparaison operator */
template <typename TYPE>
bool AutoDelete<TYPE>::operator==(const AutoDelete<TYPE>& ToCompare)
{
	// Are internal pointers equal ?
	if ( pObject == ToCompare.pObject )
	{
		return true;
	}
	return false;
}

/** @brief comparaison operator */
template <typename TYPE>
bool AutoDelete<TYPE>::operator!=(const TYPE * ToCompare)
{
	// Are internal pointer equals to the given one ?
	if ( pObject != ToCompare )
	{
		return true;
	}
	return false;
}

/** @brief comparaison operator */
template <typename TYPE>
bool AutoDelete<TYPE>::operator!=(const AutoDelete<TYPE>& ToCompare)
{
	// Are internal pointers equal ?
	if ( pObject != ToCompare.pObject )
	{
		return true;
	}
	return false;
}

} // namespace Omiscid

#endif // __AUTO_DELETE_H__
