/**
* @file SimpleList.h
* @ingroup System
* @ingroup UserFriendly
* @brief Definition of SimpleListElement class,
* SimpleList class and MutexedSimpleList class.
*/

#ifndef __SIMPLE_LIST_H__
#define __SIMPLE_LIST_H__

#include <System/ConfigSystem.h>
#include <System/SimpleListException.h>

namespace Omiscid {

template <typename TYPE> class SimpleList;

template <typename TYPE> class SimpleRecycleList;

/**
* @class SimpleListElement SimpleList.h System/SimpleList.h
* @brief Template class for cells of the list.
*
* It enable to link several between them
* The instances of this class are cells of a list.
* They contains different elements according to the template.
* They have a pointer on another cell.
* @author Dominique Vaufreydaz
*/
template <typename TYPE>
class SimpleListElement
{
	friend class SimpleList<TYPE>;
	friend class SimpleRecycleList<TYPE>;

public:
	/** @brief Default Constructor */
	SimpleListElement()
	{
		NextElement = NULL;
	}

	/** @brief Initialize the object */
	void Init()
	{
		NextElement = NULL;
	}

	/** @brief Destructor */
	virtual ~SimpleListElement();

private:
	TYPE ElementContainer; /*!< the data contains by the cell */
	SimpleListElement<TYPE> * NextElement; /*!< a pointer on the next cell in the list */
};

/** @brief Default Constructor */
template <typename TYPE>
SimpleListElement<TYPE>::~SimpleListElement()
{
}

/**
* @class SimpleList SimpleList.h System/SimpleList.h
* @brief Template class to manage list of object.
*
* This class provides methods to go through the list elements.
* It is possble to delete an element and then go to the next.
* It is possible to delete an element by giving its value.
* @author Dominique Vaufreydaz
*/
template <typename TYPE>
class SimpleList
{
public:
	/** @brief Constructor
	*
	* Build an empty list
	*/
	SimpleList();

	/** @brief Copy constructor
	*
	* Build a copy of the list.
	*/
	SimpleList(SimpleList<TYPE>& ToCopy);

	/** @brief equal operator
	*
	* Build a copy of the list.
	*/
	SimpleList& operator=(SimpleList<TYPE>& ToCopy);

	/** @brief Destructor */
	virtual ~SimpleList();

	/** @brief Function to add an item at tail of the list
	* @param Val [in] the item to add
	* @return false if a new cell has not been allocated
	*/
	bool Add( const TYPE& Val );

	/** @brief Function to add an item at head of the list
	* @param Val [in] the item to add
	* @return false if a new cell has not been allocated
	*/
	bool AddHead( const TYPE& Val );

	/** @brief Function to add an item at tail of the list
	* @param Val [in] the item to add
	* @return false if a new cell has not been allocated
	*/
	bool AddTail( const TYPE& Val );

	/** @brief Retrieve the current number of elements in the list
	* @return the current number of elements
	*/
	unsigned int GetNumberOfElements() const;

	/** @name First, Next, GetCurrent, AtEnd, NotAtEnd RemoveCurrent */
	//@{
	/** @brief Set position to the first element */
	void First();

	/** @brief Set position to the next element
	* @return false if there is not a next element : it is the end of the list.
	*/
	bool Next();

	/** @brief Do we reach the end of the list
	* @return true is we have reached the end of the list
	*/
	bool AtEnd() const;

	/** @brief Are we at the begining of the middle of the list ?
	* @return true is we don't have reached the end of the list
	*/
	bool NotAtEnd() const;

	/** @brief Get the current element of the list
	* @return the current element of the list
	* @exception SimpleListException raised if GetCurrent is called after a call to RemoveCurrent
	*/
	TYPE& GetCurrent() const;

	/** @brief Remove the current element
	* @return false if called on an empty list
	* @exception SimpleListException raised if this method has already been called on the same element
	*/
	bool RemoveCurrent();
	//@}

	/** @brief Get the first element of the list.
	* @return the first element of the list
	* @exception SimpleListException raised if the list is empty
	*/
	TYPE& GetFirst() const;

	/** @brief Get the first element of the list. Equivalent to GetFirst()
	* @return the first element of the list
	* @exception SimpleListException raised if the list is empty
	*/
	inline TYPE& GetHead() const { return GetFirst(); };

	/** @brief Get the last element of the list.
	* @return the last element of the list
	* @exception SimpleListException raised if the list is empty
	*/
	TYPE& GetLast() const;

	/** @brief Get the first element of the list. Equivalent to GetFirst()
	* @return the first element of the list
	* @exception SimpleListException raised if the list is empty
	*/
	inline TYPE& GetTail() const { return GetLast(); };

	/** @brief Test if the list is empty or not
	* @return true if the list is empty
	*/
	bool IsEmpty() const;

	/** @brief Test if the list is empty or not
	* @return true if the list is not empty
	*/
	bool IsNotEmpty() const;

	/** @brief Extract the first element of the list
	*
	* Return the first element in the list and remove it from the list
	* @return the first element in the list
	* @exception  SimpleListException raised if the list is empty
	*/
	TYPE ExtractFirst();

	/** @brief Remove a specific element
	* @param Element [in] the element to remove
	* @return false if the element to delete has not been found in the list
	*/
	bool Remove(const TYPE& Element);


	/** @brief  Empty (so empty) the whole list */
	void Empty();


protected:
	/** @brief Obtain a new SimpleListElement object
	*
	* Create a new instance of SimpleListElement object
	*/
	SimpleListElement<TYPE>* GetNewSimpleListElement() const;

	/** @brief Release a SimpleListElement object
	*
	* Delete the SimpleListElement object
	* @param elt the element to release
	*/
	void ReleaseSimpleListElement(SimpleListElement<TYPE>* elt) const;

   /** @brief Init Myself
	*
	*/
	void Init()
	{
		// There is nothing in the list
		NumberOfElements = 0;
		Head = NULL;
		Tail = NULL;

		// There is no position at all
		PreviousElement = NULL;
		CurrentElement = NULL;
		RemoveCurrentHasOccured = false;
	}

private:
	SimpleListElement<TYPE> * Head, * Tail; /*!< pointers on head and tail of the list */
	unsigned int NumberOfElements; /*!< number of elements in the list*/

	SimpleListElement<TYPE> * PreviousElement, * CurrentElement; /*!< pointer on list cells */
	bool RemoveCurrentHasOccured; /*!< set to the value 'true' after a call to the method RemoveCurrent */
};

template <typename TYPE>
SimpleList<TYPE>::SimpleList()
{
	Init();
}

/** @brief Copy constructor
*
* Build a copy of the list.
*/
template <typename TYPE>
SimpleList<TYPE>::SimpleList(SimpleList<TYPE>& ToCopy)
{
	Init();

	// From the first to the last
	for( ToCopy.First(); ToCopy.NotAtEnd(); ToCopy.Next() )
	{
		// Add tail in order to preserve the order of the list ToCopy
		AddTail(ToCopy.GetCurrent());
	}
}

/** @brief equal operator
*
* Build a copy of the list.
*/
template <typename TYPE>
SimpleList<TYPE>& SimpleList<TYPE>:: operator=(SimpleList<TYPE>& ToCopy)
{
	Empty();

	// From the first to the last
	for( ToCopy.First(); ToCopy.NotAtEnd(); ToCopy.Next() )
	{
		// Add tail in order to preserve the order of the list ToCopy
		AddTail(ToCopy.GetCurrent());
	}
}


template <typename TYPE>
SimpleList<TYPE>::~SimpleList()
{
	Empty();
}

// Access fonction
template <typename TYPE>
bool SimpleList<TYPE>::Add( const TYPE& Val )
{
	SimpleListElement<TYPE> * tmp = GetNewSimpleListElement();
	if ( tmp == NULL ) // Plus assez de memoire
		return false;

	tmp->ElementContainer = Val;
	if ( Head == NULL ) // La liste etait vide
	{
		Tail =	Head = tmp;
	}
	else
	{
		// Le nouveau devient le dernier
		Tail->NextElement = tmp;
		Tail = tmp;

		//if ( Head->NextElement == NULL )
		//{
		//	Head->NextElement = tmp;
		//}
	}

	// On incremente le nombre d'objet dans la liste
	NumberOfElements++;

	// Tout est ok
	return true;
}

/** @brief Function to add an item at head of the list
* @param Val [in] the item to add
* @return false if a new cell has not been allocated
*/
template <typename TYPE>
bool SimpleList<TYPE>::AddHead( const TYPE& Val )
{
	SimpleListElement<TYPE> * tmp = GetNewSimpleListElement();
	if ( tmp == NULL ) // Plus assez de memoire
		return false;

	tmp->ElementContainer = Val;
	if ( Head == NULL ) // La liste etait vide
	{
		Tail =	Head = tmp;
	}
	else
	{
		// Le nouveau devient le dernier
		// Head->NextElement = tmp;
		tmp->NextElement = Head;
		Head = tmp;
	}

	// On incremente le nombre d'objet dans la liste
	NumberOfElements++;

	// Tout est ok
	return true;
}

/** @brief Function to add an item at tail of the list
* @param Val [in] the item to add
* @return false if a new cell has not been allocated
*/
template <typename TYPE>
bool SimpleList<TYPE>::AddTail( const TYPE& Val )
{
	// just call the add function
	return Add(Val);
}

template <typename TYPE>
unsigned int SimpleList<TYPE>::GetNumberOfElements() const
{
	return NumberOfElements;
}

// Set position to the first element
template <typename TYPE>
void SimpleList<TYPE>::First()
{
	PreviousElement = NULL;
	CurrentElement = Head;
	RemoveCurrentHasOccured = false;
}

// Set position to the next element
template <typename TYPE>
bool SimpleList<TYPE>::Next()
{
	// Ok, we do not have any other element
	if ( CurrentElement == NULL )
	{
		RemoveCurrentHasOccured = false;
		return false;
	}

	//seb
	if ( RemoveCurrentHasOccured == true )
	{
		RemoveCurrentHasOccured = false;
		// Previous and Current have already the good value
		return true;
	}

	// Here we can go forward in the list
	PreviousElement = CurrentElement;
	CurrentElement  = CurrentElement->NextElement;

	// Have we reach the end of the list ?
	if ( CurrentElement == NULL )
	{
		return false;
	}

	return true;
}

// Set position to the next element
template <typename TYPE>
bool SimpleList<TYPE>::AtEnd() const
{
	// Ok, we do not have any other element
	if ( CurrentElement == NULL )
		return true;

	return false;
}

// Set position to the next element
template <typename TYPE>
bool SimpleList<TYPE>::NotAtEnd() const
{
	// Ok, we do not have any other element
	if ( CurrentElement != NULL )
		return true;

	return false;
}

// Get the current element of the list
template <typename TYPE>
TYPE& SimpleList<TYPE>::GetCurrent() const
{
	if ( RemoveCurrentHasOccured == true )
	{
		throw SimpleListException("SimpleList<TYPE>::GetCurrent : Forbidden after a call to RemoveCurrent");
	}

#ifdef DEBUG
	if ( CurrentElement== NULL )
	{
		OmiscidError( "Warning :  In SimpleList<TYPE>::GetCurrent() CurrentElement == NULL\n" );
	}
#endif

	return CurrentElement->ElementContainer;
}

// Remove the current element
template <typename TYPE>
bool SimpleList<TYPE>::RemoveCurrent()
{
	if ( RemoveCurrentHasOccured == true )
	{
		throw SimpleListException("SimpleList<TYPE>::RemoveCurrent : Forbidden after a previous call to RemoveCurrent");
	}
	RemoveCurrentHasOccured = true;
	SimpleListElement<TYPE> * tmp = CurrentElement;

	if ( tmp == NULL )
	{
		// We must call at least First on a non empty list
		return false;
	}

	// Here we have a current element...
	// Are we at the begining of the SimpleList ?
	if ( Head == tmp )
	{
		// Yes, wa are !
		Head = tmp->NextElement;

		// PreviousElement remains the same :
		// PreviousElement == NULL

		// The new current element if the next in the list, so the head
		CurrentElement = Head;
	}
	else
	{
		// We are not at the begining of the list
		// Here we can remove directly the element within the list
		tmp = CurrentElement;
		PreviousElement->NextElement = tmp->NextElement;
		CurrentElement = tmp->NextElement;
	}

	// Was the element the last item of the list ?
	if ( Tail == tmp )
	{
		// We have remove the last item of the list
		Tail = PreviousElement;
	}

	// Delete the old current element
	ReleaseSimpleListElement(tmp);

	// Change the element count
	NumberOfElements--;

	return true;
}

// Remove a specific element
template <typename TYPE>
bool SimpleList<TYPE>::Remove(const TYPE& Element)
{
	for( First(); NotAtEnd(); Next() )
	{
		if ( GetCurrent() == Element )
		{
			RemoveCurrent();
			return true;
		}
	}
	return false;
}

template <typename TYPE>
void SimpleList<TYPE>::Empty()
{
	SimpleListElement<TYPE> * tmp;
	// On libere tous les elements de la liste
	while( Head != NULL )
	{
		tmp = Head;
		Head = Head->NextElement; // On dechaine le premier element
		ReleaseSimpleListElement(tmp);
		NumberOfElements--; // Non obligatoire !
	}

	Init();
}

template <typename TYPE>
TYPE& SimpleList<TYPE>::GetFirst() const
{
	if ( Head == NULL )
	{
		throw SimpleException( "Could not call GetFirst() or GetHead() on a empty list" );
	}

	return Head->ElementContainer;
}

template <typename TYPE>
TYPE& SimpleList<TYPE>::GetLast() const
{
	if ( Tail == NULL )
	{
		throw SimpleException( "Could not call GetLast() or GetTail() on a empty list" );
	}

	return Tail->ElementContainer;
}

template <typename TYPE>
bool SimpleList<TYPE>::IsEmpty() const
{
	return Head == NULL;
}

template <typename TYPE>
bool SimpleList<TYPE>::IsNotEmpty() const
{
	return Head != NULL;
}

template <typename TYPE>
TYPE SimpleList<TYPE>::ExtractFirst()
{
	if ( IsEmpty() == true )
	{
		throw SimpleListException("SimpleList<TYPE>::ExtractFirst : Forbidden when the list is empty");
	}

	SimpleListElement<TYPE>* elt = Head;
	if ( Head == Tail )
	{
		Head = NULL;
		Tail = NULL;
	}
	else
	{
		Head = elt->NextElement;
	}
	NumberOfElements--;

	TYPE tmp = elt->ElementContainer;
	ReleaseSimpleListElement(elt);
	return tmp;
}

template <typename TYPE>
SimpleListElement<TYPE>*  SimpleList<TYPE>::GetNewSimpleListElement() const
{
	return new OMISCID_TLM SimpleListElement<TYPE>;
}

template <typename TYPE>
void SimpleList<TYPE>::ReleaseSimpleListElement(SimpleListElement<TYPE>* elt) const
{
	delete elt;
}

} // namespace Omiscid

#endif // __SIMPLE_LIST_H__

