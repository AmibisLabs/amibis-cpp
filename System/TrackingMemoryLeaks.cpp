
// #include <System/TrackingMemoryLeaks.h>

#ifdef TRACKING_MEMORY_LEAKS

#if defined WIN32 || defined _WIN32
	#define _CRT_SECURE_NO_DEPRECATE

	#include <windows.h>
	#include <winbase.h>

	#define snprintf _snprintf
#else
namespace Omiscid {

	void AddMemoryBlock(void* addr,  size_t asize,  const char *fname, int lnum);
	void RemoveMemoryBlock(void* addr);

	inline void OutputDebugString(const char * OutputString)
	{
		if ( OutputString )
		{
			fprintf( stderr, OutputString );
		}
	}

} // namespace Omiscid
#endif

#include <stdio.h>
#include <stdlib.h>


// Local Omiscid declaration
namespace Omiscid {

template <typename TYPE> class MemoryList;

/**
 * @class SimpleListElement MemoryList.h System/MemoryList.h
 * \brief Template class for cells of the list.
 *
 * It enable to link several between them 
 * The instances of this class are cells of a list.
 * They contains different elements according to the template.
 * They have a pointer on another cell.
 * \author Dominique Vaufreydaz
 */
template <typename TYPE>
class SimpleListElement
{
	friend class MemoryList<TYPE>; 	

public:
	/** \brief Default Constructor */
	SimpleListElement()
	{
		NextElement = NULL;
	}

	/** \brief Initialize the object */
	void Init()
	{
	  	NextElement = NULL;
	}

	/** \brief Destructor */
	virtual ~SimpleListElement();

private:
	TYPE ElementContainer; /*!< the data contains by the cell */
 	SimpleListElement<TYPE> * NextElement; /*!< a pointer on the next cell in the list */
};

/** \brief Default Constructor */
template <typename TYPE>
SimpleListElement<TYPE>::~SimpleListElement()
{
}

/**
 * @class MemoryList MemoryList.h System/MemoryList.h
 * @brief Template class to manage list of object.
 * 
 * This class provides methods to go through the list elements.
 * It is possble to delete an element and then go to the next.
 * It is possible to delete an element by giving its value. 
 * \author Dominique Vaufreydaz
 */
template <typename TYPE>
class MemoryList  
{
public:
        /** \brief Constructor 
	 *
	 * Build an empty list
	 */
	MemoryList();

	/** \brief Destructor */
	virtual ~MemoryList();

	/** \brief Function to add an item at tail of the list 
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
	bool Add( const TYPE& Val );

	/** \brief Function to add an item at head of the list 
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
	bool AddHead( const TYPE& Val );

	/** \brief Function to add an item at tail of the list 
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
	bool AddTail( const TYPE& Val );

	/** \brief Retrieve the current number of elements in the list
	 * \return the current number of elements
	 */
	unsigned int GetNumberOfElements() const;

	/** \name First, Next, GetCurrent, AtEnd, NotAtEnd RemoveCurrent */
	//@{
	/** \brief Set position to the first element */
	void First();

	/** \brief Set position to the next element 
	 * \return false if there is not a next element : it is the end of the list.
	 */
	bool Next();

	/** \brief Do we reach the end of the list 
	 * \return true is we have reached the end of the list
	 */
	bool AtEnd() const;

	/** \brief Are we at the begining of the middle of the list ? 
	 * \return true is we don't have reached the end of the list
	 */
	bool NotAtEnd() const;

	/** \brief Get the current element of the list 
	 * \return the current element of the list 
	 * \exception SimpleListException raised if GetCurrent is called after a call to RemoveCurrent
	 */
	TYPE& GetCurrent() const;

	/** \brief Remove the current element
	 * \return false if called on an empty list
	 * \exception SimpleListException raised if this method has already been called on the same element
	 */
	bool RemoveCurrent();
	//@}

	/** \brief Test if the list is empty or not 
	 * \return true if the list is empty
	 */
	bool IsEmpty() const;	

	/** \brief Test if the list is empty or not 
	 * \return true if the list is not empty
	 */
	bool IsNotEmpty() const;

	/** \brief Extract the first element of the list 
	 *
	 * Return the first element in the list and remove it from the list
	 * \return the first element in the list
	 * \exception  SimpleListException raised if the list is empty
	 */
	TYPE ExtractFirst();

	/** \brief Remove a specific element 
	 * \param Element [in] the element to remove
	 * \return false if the element to delete has not been found in the list
	 */
	bool Remove(const TYPE& Element);


	/** \brief  Empty (so empty) the whole list */
	virtual void Empty();


 protected:
	/** \brief Obtain a new SimpleListElement object 
	 *
	 * Create a new instance of SimpleListElement object 
	 */
	virtual SimpleListElement<TYPE>* GetNewSimpleListElement() const;

	/** \brief Release a SimpleListElement object 
	 *
	 * Delete the SimpleListElement object 
	 * @param elt the element to release
	 */
	virtual void ReleaseSimpleListElement(SimpleListElement<TYPE>* elt) const;
	
 private:
	SimpleListElement<TYPE> * Head, * Tail; /*!< pointers on head and tail of the list */
	unsigned int NumberOfElements; /*!< number of elements in the list*/

	SimpleListElement<TYPE> * PreviousElement, * CurrentElement; /*!< pointer on list cells */
	bool RemoveCurrentHasOccured; /*!< set to the value 'true' after a call to the method RemoveCurrent */
};

template <typename TYPE>
MemoryList<TYPE>::MemoryList()
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

template <typename TYPE>
MemoryList<TYPE>::~MemoryList()
{
	Empty();
}

// Access fonction
template <typename TYPE>
bool MemoryList<TYPE>::Add( const TYPE& Val )
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

	/** \brief Function to add an item at head of the list 
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
template <typename TYPE>
bool MemoryList<TYPE>::AddHead( const TYPE& Val )
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

	/** \brief Function to add an item at tail of the list 
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
template <typename TYPE>
bool MemoryList<TYPE>::AddTail( const TYPE& Val )
{
	// just call the add function
	return Add(Val);
}

template <typename TYPE>
unsigned int MemoryList<TYPE>::GetNumberOfElements() const
{
	return NumberOfElements;
}

// Set position to the first element
template <typename TYPE>
void MemoryList<TYPE>::First()
{
	PreviousElement = NULL;
	CurrentElement = Head;
	RemoveCurrentHasOccured = false;
}

// Set position to the next element
template <typename TYPE>
bool MemoryList<TYPE>::Next()
{
	// Ok, we do not have any other element
	if ( CurrentElement == NULL ){
		RemoveCurrentHasOccured = false;
		return false;
	}

	//seb
	if(RemoveCurrentHasOccured){
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
bool MemoryList<TYPE>::AtEnd() const
{
	// Ok, we do not have any other element
	if ( CurrentElement == NULL )
		return true;

	return false;
}

// Set position to the next element
template <typename TYPE>
bool MemoryList<TYPE>::NotAtEnd() const
{
	// Ok, we do not have any other element
	if ( CurrentElement != NULL )
		return true;

	return false;
}

// Get the current element of the list
template <typename TYPE>
TYPE& MemoryList<TYPE>::GetCurrent() const
{
	if ( RemoveCurrentHasOccured )
		throw ("RemoveCurrentHasOccured");

	return CurrentElement->ElementContainer;
}

// Remove the current element
template <typename TYPE>
bool MemoryList<TYPE>::RemoveCurrent()
{
	if(RemoveCurrentHasOccured)
		throw ("RemoveCurrentHasOccured");

	RemoveCurrentHasOccured = true;
	SimpleListElement<TYPE> * tmp = CurrentElement;

	if ( tmp == NULL )
	{
		// We must call at least First on a non empty list
		return false;
	}

	// Here we have a current element...
	// Are we at the begining of the MemoryList ?
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
bool MemoryList<TYPE>::Remove(const TYPE& Element)
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
void MemoryList<TYPE>::Empty()
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
}

template <typename TYPE>
bool MemoryList<TYPE>::IsEmpty() const
{ return Head == NULL; }

template <typename TYPE>
bool MemoryList<TYPE>::IsNotEmpty() const
{ return Head != NULL; }

template <typename TYPE>
TYPE MemoryList<TYPE>::ExtractFirst()
{
  if(IsEmpty())
    throw SimpleListException("MemoryList<TYPE>::ExtractFirst : Forbidden when the list is empty");

  SimpleListElement<TYPE>* elt = Head;
  if(Head == Tail)
    {
      Head = NULL; Tail = NULL;
    }else{
      Head = elt->NextElement;
    }
  NumberOfElements--;

  TYPE tmp = elt->ElementContainer;
  ReleaseSimpleListElement(elt);
  return tmp;
}

template <typename TYPE>
SimpleListElement<TYPE>*  MemoryList<TYPE>::GetNewSimpleListElement() const
{ return new SimpleListElement<TYPE>; }

template <typename TYPE>
void MemoryList<TYPE>::ReleaseSimpleListElement(SimpleListElement<TYPE>* elt) const
{ delete elt; }


void StartTrackingMemoryLeaks();
void StopTrackingMemoryLeaks();

void AddMemoryBlock(void* addr,  size_t asize,  const char *fname, int lnum);
void RemoveMemoryBlock(void* addr);

typedef enum TrackMemoryValues{
	FileNameSize = 255,
	TemporaryBufferSize = 4*1024-1	// 4 Kb - 1 byte
};

void DumpUnfreed();

static bool Tracking = false;

class MemoryBlockInfos
{
public:
	char	Filename[FileNameSize+1];
	void*	Where;
	size_t	Size;
	int		Line;
	MemoryBlockInfos * Next;

	MemoryBlockInfos()
	{
		Filename[0] = '\0';
		Where = NULL;
		Size = 0;
		Line = -1;
		Next = NULL;
	}
};

MemoryList<MemoryBlockInfos *> AllocList;

static char WriteSizeBuffer[TemporaryBufferSize+1];

static char * PrintSize( unsigned int SizeOfData );

} // namespace Omiscid

using namespace Omiscid;

void * operator new( size_t size, int line, const char *file )
{
	void *ptr = (void *)malloc(size);
	Omiscid::AddMemoryBlock(ptr, size, file, line);
	return(ptr);
}

void * operator new[]( size_t size, int line, const char *file )
{
	void *ptr = (void *)malloc(size);
	Omiscid::AddMemoryBlock(ptr, size, file, line);
	return(ptr);
}

void operator delete(void *p)
{
	Omiscid::RemoveMemoryBlock(p);
	free(p);
}

void operator delete[](void *p)
{
	Omiscid::RemoveMemoryBlock(p);
	free(p);
}

void Omiscid::StartTrackingMemoryLeaks()
{
	Tracking = true;
}

void Omiscid::StopTrackingMemoryLeaks()
{
	Tracking = false;
}

void Omiscid::AddMemoryBlock(void* addr,  size_t aSize,  const char *fname, int lnum)
{
	MemoryBlockInfos *info;

	if ( Tracking == false )
	{
		return;
	}

	info = new MemoryBlockInfos;

	if ( info == NULL )
	{
		// Could not allocate list, disable tracking
		Tracking = false;
		return;

	}

	// Fill information
	info->Where = addr;
	strncpy( info->Filename, fname, FileNameSize-1 );
	info->Filename[FileNameSize-1];
	info->Line = lnum;
	info->Size = aSize;
	
	// Add head
	AllocList.AddTail( info );
}

void Omiscid::RemoveMemoryBlock(void* addr)
{
	for( AllocList.First(); AllocList.NotAtEnd(); AllocList.Next() )
	{
		if( AllocList.GetCurrent()->Where == addr )
		{
			AllocList.RemoveCurrent();
			break;
		}
	}
}

static char * Omiscid::PrintSize( unsigned int SizeOfData )
{
	if ( SizeOfData < 1024 )
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%d bytes", SizeOfData);
	}
	else if ( SizeOfData < 1024*1024 )
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%.2f Kbytes", (float)SizeOfData/1024.0f);
	}
	else if ( SizeOfData < 1024*1024*1024 )
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%.2f Mbytes", (float)SizeOfData/(1024.0f*1024.0f));
	}
	else // totalSize >= 1024*1024*1024*1024
	{
		snprintf(WriteSizeBuffer, TemporaryBufferSize, "%.2f Gbytes", (float)SizeOfData/(1024.0f*1024.0f*1024.0f));
	}

	return WriteSizeBuffer;
}

void Omiscid::DumpUnfreed()
{
	unsigned int TotalSize = 0;
	unsigned int NbBlocks = 0;
	char buf[TemporaryBufferSize+1];	   

	if ( AllocList.GetNumberOfElements() == 0 )
	{
		return;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	MemoryBlockInfos * pTmp;

	for( AllocList.First(); AllocList.NotAtEnd(); AllocList.Next() )
	{
		NbBlocks++;

		pTmp = AllocList.GetCurrent();

		// Visual studio compliant output (you can click on it to see the rigth line
		snprintf(buf, TemporaryBufferSize+1, "%s(%d) : %s unfreed memory at address %p\n", pTmp->Filename, pTmp->Line, PrintSize((unsigned int)pTmp->Size), pTmp->Where );
		OutputDebugString(buf);

		void * tmpv = pTmp->Where;
		char * tmpc = (char*)tmpv;
		int lSize = (int)pTmp->Size;
		for( int j = 0; j < lSize && j < 20; j++)
		{
			snprintf(buf, TemporaryBufferSize+1, "%c", tmpc[j] );
			OutputDebugString(buf);
		}
		snprintf(buf, TemporaryBufferSize+1, "\n" );
		OutputDebugString(buf);

		TotalSize += (unsigned int)lSize;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);
	
	snprintf(buf, TemporaryBufferSize+1, "Total memory unfreed %s (%u allocated blocks).\n", PrintSize(TotalSize), NbBlocks );
	OutputDebugString(buf);

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	// Free the memory
	for( AllocList.First(); AllocList.NotAtEnd(); AllocList.Next() )
	{
		delete AllocList.GetCurrent();
	}
}

namespace Omiscid {

class UnfreedPrint
{
public:
	~UnfreedPrint()
	{
		DumpUnfreed();
	}
};

static UnfreedPrint PrintUnfreed;

} // namespace Omiscid

#endif // TRACKING_MEMORY_LEAKS

