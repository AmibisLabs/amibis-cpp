
#include <System/TrackingMemoryLeaks.h>

#include <System/Mutex.h>

#ifdef TRACKING_MEMORY_LEAKS

#if defined WIN32 || defined _WIN32
	#define _CRT_SECURE_NO_DEPRECATE

	#include <windows.h>
	#include <winbase.h>

	#define snprintf _snprintf
#else
namespace Omiscid {
	inline void OutputDebugString(const char * OutputString)
	{
		if ( OutputString )
		{
			fprintf( stderr, OutputString );
		}
	}

} // namespace Omiscid
#endif

using namespace Omiscid;

namespace Omiscid {
	Mutex TrackingMemoryLocker;
	unsigned int NumberOfTrackingMemoryClient = 0;
}	// namespace Omiscid

TrackMemoryLeaks::TrackMemoryLeaks()
{
	TrackingMemoryLocker.EnterMutex();
	NumberOfTrackingMemoryClient++;
	TrackingMemoryLocker.LeaveMutex();
}

TrackMemoryLeaks::~TrackMemoryLeaks()
{
	TrackingMemoryLocker.EnterMutex();
	NumberOfTrackingMemoryClient--;
	TrackingMemoryLocker.LeaveMutex();

	// Dump unfreed will lock data...
	DumpUnfreed();
}

namespace Omiscid {

	void AddMemoryBlock(size_t asize, void** addr);
	void RemoveMemoryBlock(void* addr);
} // namespace Omiscid

#include <stdio.h>
#include <stdlib.h>


using namespace Omiscid;

// Local Omiscid declaration
namespace Omiscid {

/**
 * \author Dominique Vaufreydaz
 */
struct SimpleMemoryElement
{
	/*!< the data contains by the cell */
	void*	Where;			// Given pointer returned by new
	size_t	Size;			// Original size of block
	 struct SimpleMemoryElement * NextElement; /*!< a pointer on the next cell in the list */
};

/**
 * @class MemoryList MemoryList.h System/MemoryList.h
 * @brief class to manage list of allocated memory blocks.
 *
 * \author Dominique Vaufreydaz
 */
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

	/** \brief Function to add a SimpleMemoryElement
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
	bool Add( size_t SizeOfBlock );

	/** \brief Function to add a SimpleMemoryElement
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
	bool AddHead( size_t SizeOfBlock );

	/** \brief Function to add a SimpleMemoryElement
	 * \param Val [in] the item to add
	 * \return false if a new cell has not been allocated
	 */
	bool AddTail( size_t SizeOfBlock );

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
	SimpleMemoryElement * GetCurrent() const;

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
	SimpleMemoryElement * ExtractFirst();

	/** \brief Remove a specific element
	 * \param Element [in] the element to remove
	 * \return false if the element to delete has not been found in the list
	 */
	bool Remove(void * LegalPointer);


	/** \brief  Empty (so empty) the whole list */
	void Empty();


 protected:
	/** \brief Obtain a new struct SimpleMemoryElement object
	 *
	 * Create a new instance of struct SimpleMemoryElement object
	 */
	struct SimpleMemoryElement* GetNewSimpleListElement(size_t SizeOfBlock) const;

	/** \brief Release a struct SimpleMemoryElement object
	 *
	 * Delete the struct SimpleMemoryElement object
	 * @param elt the element to release
	 */
	void ReleaseSimpleListElement(struct SimpleMemoryElement* elt) const;

 public:
	struct SimpleMemoryElement * Head, * Tail; /*!< pointers on head and tail of the list */
	unsigned int NumberOfElements; /*!< number of elements in the list*/

	struct SimpleMemoryElement * PreviousElement, * CurrentElement; /*!< pointer on list cells */
	bool RemoveCurrentHasOccured; /*!< set to the value 'true' after a call to the method RemoveCurrent */
};

MemoryList::MemoryList()
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

MemoryList::~MemoryList()
{
	Empty();
}

// Access fonction
bool MemoryList::Add( size_t SizeOfBlock )
{
	struct SimpleMemoryElement * tmp = GetNewSimpleListElement(SizeOfBlock);
	if ( tmp == NULL )
	{
		return false;
	}

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
bool MemoryList::AddHead( size_t SizeOfBlock )
{
	struct SimpleMemoryElement * tmp = GetNewSimpleListElement(SizeOfBlock);
	if ( tmp == NULL )
	{
		return false;
	}

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
bool MemoryList::AddTail( size_t SizeOfBlock )
{
	// just call the add function
	return Add(SizeOfBlock);
}

unsigned int MemoryList::GetNumberOfElements() const
{
	return NumberOfElements;
}

// Set position to the first element
void MemoryList::First()
{
	PreviousElement = NULL;
	CurrentElement = Head;
	RemoveCurrentHasOccured = false;
}

// Set position to the next element
bool MemoryList::Next()
{
	// Ok, we do not have any other element
	if ( CurrentElement == NULL ){
		RemoveCurrentHasOccured = false;
		return false;
	}

	//seb
	if(RemoveCurrentHasOccured)
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
bool MemoryList::AtEnd() const
{
	// Ok, we do not have any other element
	if ( CurrentElement == NULL )
		return true;

	return false;
}

// Set position to the next element
bool MemoryList::NotAtEnd() const
{
	// Ok, we do not have any other element
	if ( CurrentElement != NULL )
		return true;

	return false;
}

// Get the current element of the list
struct SimpleMemoryElement * MemoryList::GetCurrent() const
{
	if ( RemoveCurrentHasOccured )
		throw  "RemoveCurrentHasOccured";

	return CurrentElement;
}

// Remove the current element
bool MemoryList::RemoveCurrent()
{
	if(RemoveCurrentHasOccured)
		throw  "RemoveCurrentHasOccured";

	RemoveCurrentHasOccured = true;
	struct SimpleMemoryElement * tmp = CurrentElement;

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

	// free the old current element
	ReleaseSimpleListElement(tmp);

	// Change the element count
	NumberOfElements--;

	return true;
}

// Remove a specific element
bool MemoryList::Remove(void * LegalPointer)
{
	char * Element = (char*)LegalPointer;

	for( First(); NotAtEnd(); Next() )
	{
		if ( GetCurrent()->Where == Element )
		{
			RemoveCurrent();
			return true;
		}
	}
	return false;
}

struct SimpleMemoryElement* MemoryList::GetNewSimpleListElement(size_t SizeOfBlock) const
{
	// Allocate the asked size + the size of our structure
	struct SimpleMemoryElement * tmp = (struct SimpleMemoryElement*)malloc(SizeOfBlock+sizeof(struct SimpleMemoryElement));

	if ( tmp != NULL )
	{
		tmp->Where = (((char*)tmp)+sizeof(struct SimpleMemoryElement));
		tmp->Size = SizeOfBlock;
		tmp->NextElement = NULL;
	}

	return tmp;
}

void MemoryList::ReleaseSimpleListElement(struct SimpleMemoryElement* elt) const
{
	if ( elt )
	{
		free(elt);
	}
}

void MemoryList::Empty()
{
	struct SimpleMemoryElement * tmp;
	// On libere tous les elements de la liste
	while( Head != NULL )
	{
		tmp = Head;
		Head = Head->NextElement; // On dechaine le premier element
		ReleaseSimpleListElement(tmp);
		NumberOfElements--; // Non obligatoire !
	}
}

bool MemoryList::IsEmpty() const
{ return Head == NULL; }

bool MemoryList::IsNotEmpty() const
{ return Head != NULL; }

struct SimpleMemoryElement * MemoryList::ExtractFirst()
{
  if(IsEmpty())
	throw "MemoryList::ExtractFirst : Forbidden when the list is empty";

  struct SimpleMemoryElement * elt = Head;
  if(Head == Tail)
	{
	  Head = NULL; Tail = NULL;
	}else{
	  Head = elt->NextElement;
	}
  NumberOfElements--;

  return elt;
}

void AddMemoryBlock(size_t asize,void** addr);
void RemoveMemoryBlock(void* addr);

typedef enum TrackMemoryValues
{
	TemporaryBufferSize = 4*1024-1	// 4 Kb - 1 byte
};

void DumpUnfreed();

MemoryList AllocList;

static char WriteSizeBuffer[TemporaryBufferSize+1];

static char * PrintSize( unsigned int SizeOfData );

} // namespace Omiscid

using namespace Omiscid;

void * operator new( size_t size ) throw (std::bad_alloc)
{
	void *ptr;
	Omiscid::AddMemoryBlock(size, &ptr);
	return(ptr);
}

void * operator new[]( size_t size ) throw (std::bad_alloc)
{
	void *ptr;
	Omiscid::AddMemoryBlock(size, &ptr);
	return(ptr);
}

void operator delete(void *p) throw ()
{
	Omiscid::RemoveMemoryBlock(p);
}

void operator delete[](void *p) throw ()
{
	Omiscid::RemoveMemoryBlock(p);
}

void * operator new( size_t size, int Line, char * File ) throw (std::bad_alloc)
{
	void *ptr;
	Omiscid::AddMemoryBlock(size, &ptr);
	return(ptr);
}

void * operator new[]( size_t size, int Line, char * File ) throw (std::bad_alloc)
{
	void *ptr;
	Omiscid::AddMemoryBlock(size, &ptr);
	return(ptr);
}

void operator delete( void *p, int Line, char * File ) throw ()
{
	Omiscid::RemoveMemoryBlock(p);
}

void operator delete[]( void *p, int Line, char * File ) throw ()
{
	Omiscid::RemoveMemoryBlock(p);
}

void Omiscid::AddMemoryBlock( size_t aSize, void** addr)
{
	TrackingMemoryLocker.EnterMutex();
	if ( NumberOfTrackingMemoryClient != 0 )
	{
		// Add head
		if ( AllocList.AddTail( aSize ) )
		{
			*addr = AllocList.Tail->Where;
		}
	}
	else
	{
		*addr = malloc(aSize);
	}
	TrackingMemoryLocker.LeaveMutex();
}

void Omiscid::RemoveMemoryBlock(void* addr)
{
	TrackingMemoryLocker.EnterMutex();
	if ( NumberOfTrackingMemoryClient == 0 )
	{
		TrackingMemoryLocker.LeaveMutex();
		return;
	}

	if ( AllocList.Remove(addr) == false )
	{
		free(addr);
	}
	TrackingMemoryLocker.LeaveMutex();
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

void TrackMemoryLeaks::DumpUnfreed()
{
	unsigned int TotalSize = 0;
	unsigned int NbBlocks = 0;
	char buf[TemporaryBufferSize+1];

	TrackingMemoryLocker.EnterMutex();

	if ( AllocList.GetNumberOfElements() == 0 )
	{
		TrackingMemoryLocker.LeaveMutex();
		return;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	struct SimpleMemoryElement * pTmp;

	for( AllocList.First(); AllocList.NotAtEnd(); AllocList.Next() )
	{
		NbBlocks++;

		pTmp = AllocList.GetCurrent();

		// Visual studio compliant output (you can click on it to see the rigth line
		snprintf(buf, TemporaryBufferSize+1, "%s unfreed memory at address %p\n", PrintSize((unsigned int)pTmp->Size), pTmp->Where );
		OutputDebugString(buf);

		void * tmpv = pTmp->Where;
		char * tmpc = (char*)tmpv;
		int lSize = (int)pTmp->Size;
		int j;
		for( j = 0; j < lSize && j < 20; j++)
		{
			snprintf(buf, TemporaryBufferSize+1, "%2.2X ", tmpc[j] & 0x000000ff );
			OutputDebugString(buf);
		}
		for( ; j < 20; j++)
		{
			snprintf(buf, TemporaryBufferSize+1, "   " );
			OutputDebugString(buf);
		}

		for( j = 0; j < lSize && j < 20; j++)
		{
			if ( tmpc[j] >= 32 && tmpc[j] <= 126 )
			{
				snprintf(buf, TemporaryBufferSize+1, "%c", tmpc[j] );
			}
			else
			{
				snprintf(buf, TemporaryBufferSize+1, "." );
			}
			OutputDebugString(buf);
		}

		snprintf(buf, TemporaryBufferSize+1, "\n" );
		OutputDebugString(buf);

		TotalSize += (unsigned int)lSize;
	}

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	snprintf(buf, TemporaryBufferSize+1, "Total memory unfreed %s (%u allocated block%s).\n", PrintSize(TotalSize), NbBlocks, NbBlocks == 1 ? "" : "s" );
	OutputDebugString(buf);

	snprintf(buf, TemporaryBufferSize+1, "-+-+-+-+-+-+-+-+\n" );
	OutputDebugString(buf);

	// Free the memory
	for( AllocList.First(); AllocList.NotAtEnd(); AllocList.Next() )
	{
		delete AllocList.GetCurrent();
		AllocList.RemoveCurrent();
	}

	TrackingMemoryLocker.LeaveMutex();
}

#endif	// defined TRACKING_MEMORY_LEAKS
