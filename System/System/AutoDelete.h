/**
 * @file System/AutoDelete.h
 * @ingroup System
 * @brief Definition of a template class for autodeleting pointers when leaving scope
 */
#ifndef __AUTO_DELETE_H__
#define __AUTO_DELETE_H__

#include <System/ConfigSystem.h>

#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class AutoDelete System/AutoDelete.h
 * @brief AutoDelete object in order to free pointers.
 *
 * Enable to manage automatically destruction of pointers when leaving scope.
 * @author Dominique Vaufreydaz
 */
template <typename TYPE>
class AutoDelete
{
public:
	/** @brief Constructor. */
	AutoDelete();

	/** @brief Constructor with initialisation (TYPE pointer). */
	AutoDelete(TYPE * ToCopy);

	/** @brief Copy constructor. Call this constructor will raise a SimpleException. */
	AutoDelete(const AutoDelete<TYPE>& ToCopy);

	/** @brief Virtual destructor. */
	virtual ~AutoDelete();

	/** @brief Explicitally destroy the pointed object. */
	void Delete();

	/** @brief Access function true '*' operator. */
	TYPE& operator*();

	/** @brief Access function true '->' operator. */
	TYPE* operator->();

	/** @brief Get internal pointer */
	TYPE* Get()
	{
		return pObject;
	}

	/** @brief affectation operator/copy : not allowed. Call this method will raise a SimpleException. */
	const AutoDelete<TYPE>& operator=(const AutoDelete<TYPE>& ToCopy);

	/** @brief Affectation operator with a TYPE pointer. If there is already a pointer kept by this AutoDelete, a Simple Exception will raise. */
	const AutoDelete<TYPE>& operator=(TYPE * ToCopy);

	/** @brief Comparaison operator. Return true if the kept pointer is equal to ToCompare. */
	bool operator==(const TYPE * ToCompare);

	/** @brief Comparaison operator. Return true if the both kept pointers are the same (must never appear if you do not want to have multiple destructor calls !). */
	bool operator==(const AutoDelete<TYPE>& ToCompare);

	/** @brief Comparaison operator. Return true if the kept pointer is *not* equal to ToCompare. */
	bool operator!=(const TYPE * ToCompare);

	/** @brief Comparaison operator. Return true if the both kept pointers are not the same (must always be the case !). */
	bool operator!=(const AutoDelete<TYPE>& ToCompare);

private:
	TYPE * pObject;	/*!< The TYPE pointer to keep */
};

/** @brief Constructor. */
template <typename TYPE>
AutoDelete<TYPE>::AutoDelete()
{
	pObject = (TYPE*)NULL;
}

/** @brief Constructor with initialisation (TYPE pointer). */
template <typename TYPE>
AutoDelete<TYPE>::AutoDelete(TYPE * ToCopy)
{
	pObject = ToCopy;
}

/** @brief Copy constructor. Call this constructor will raise a SimpleException. */
template <typename TYPE>
AutoDelete<TYPE>::AutoDelete(const AutoDelete<TYPE>& ToCopy)
{
	throw SimpleException( "Copying AutoDelete object will result in multiple delete calls on the same pointer", 0 );
}

/** @brief Destructor. */
template <typename TYPE>
AutoDelete<TYPE>::~AutoDelete()
{
	Delete();
}

/** @brief Explicitally destroy the pointed object. */
template <typename TYPE>
void AutoDelete<TYPE>::Delete()
{
	if ( pObject != (TYPE*)NULL )
	{
		delete pObject;
		pObject = (TYPE*)NULL;
	}
}

/** @brief Access function true '*' operator. */
template <typename TYPE>
TYPE& AutoDelete<TYPE>::operator*()
{
	return *pObject;
}

/** @brief Access function true '->' operator. */
template <typename TYPE>
TYPE* AutoDelete<TYPE>::operator->()
{
	return pObject;
}

/** @brief Affectation operator/copy : not allowed. Call this method will raise a SimpleException. */
template <typename TYPE>
const AutoDelete<TYPE>& AutoDelete<TYPE>::operator=(const AutoDelete<TYPE>& ToCopy)
{
	throw SimpleException( "Copying AutoDelete object will result in multiple delete calls on the same pointer", 0 );
	return *this;
}

/** @brief Affectation operator with a TYPE pointer. If there is already a pointer kept by this AutoDelete, a Simple Exception will raise. */
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

/** @brief Comparaison operator. Return true if the kept pointer is equal to ToCompare. */
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

/** @brief Comparaison operator. Return true if the both kept pointers are the same (must never appear if you do not want to have multiple destructor calls !). */
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

/** @brief Comparaison operator. Return true if the kept pointer is *not* equal to ToCompare. */
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

/** @brief Comparaison operator. Return true if the both kept pointers are not the same (must always be the case !). */
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
