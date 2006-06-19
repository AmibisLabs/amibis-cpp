#include <System/Memory.h>

InternalMemoryBuffer::InternalMemoryBuffer()
{
	Buffer = NULL;
}

InternalMemoryBuffer::InternalMemoryBuffer(InternalMemoryBuffer&Copy)
{
	Copy.AddReference();
	Buffer = Copy.Buffer;
	Length = 
}

/** @brief Destructor
*
* free the buffer of character allocated for the string
*/
InternalMemoryBuffer::~InternalMemoryBuffer()
{
	if ( nbReference
}

/** \return the pointer on the buffer of characters */
char* InternalMemoryBuffer::GetDataPtr() const;

/** \return the length of the string */
unsigned int InternalMemoryBuffer::GetLength() const;

/** \brief add one reference to the string 
* \return the number of reference
*/
int InternalMemoryBuffer::AddReference();

/** \brief remove a reference on the string 
* \return the number of reference existing yet
*/
int InternalMemoryBuffer::RemoveReference();

/** \return the number of reference on the string */
int InternalMemoryBuffer::GetNbReference();

/** \brief Change the string contained by the object if possible
*
* The string is changed if only a reference exist on this object.
* \return true if the string has changed.
*/
bool InternalMemoryBuffer::ChangeData(const char* str);

/** \name Comparaison */
//@{
/** \return if the string contained by this object match the string contained in 'sd'*/
bool InternalMemoryBuffer::Equals(const InternalMemoryBuffer& sd) const;

/** \return if the string contained by this object match the string 'str'*/
bool InternalMemoryBuffer::Equals(const char* str) const;	

/** \return if the string contained by this object do not match the string contained in 'sd'*/
bool InternalMemoryBuffer::NotEquals(const InternalMemoryBuffer& sd) const;

/** \return if the string contained by this object do not match the string 'str''*/
bool InternalMemoryBuffer::NotEquals(const char* str) const;	
//@}

MemoryBuffer::~MemoryBuffer()
{
	if ( OriginalAlloc != 0 )
	{
		delete OriginalAlloc;

		OriginalAlloc = NULL;
		OriginalSize  = 0;
		CurrentPointer = NULL;
		CurrentSize    = 0;
	}
}

char MemoryBuffer::operator[](int Pos)
{
	if ( Pos < 0 || Pos >= (int)CurrentSize )
	{
		throw "Out of band";
	}
	return CurrentPointer[Pos];
}

const MemoryBuffer MemoryBuffer::operator+=(int Pos)
{
	if ( CurrentSize-Pos <= 0 )
	{
		throw "Out of band";
	}

	CurrentPointer += Pos;
	CurrentSize    -= Pos;

	return (*this);
}

const MemoryBuffer MemoryBuffer::operator+(int Pos)
{
	MemoryBuffer NewMemoryBuffer;
	NewMemoryBuffer += Pos;

	return NewMemoryBuffer;
}

MemoryBuffer::operator char*()
{
	return (char*)CurrentPointer;
}

MemoryBuffer::operator const char *()
{
	return (const char*)CurrentPointer;
}

MemoryBuffer::operator void*()
{
	return (void*)CurrentPointer;
}

MemoryBuffer::operator const void*()
{
	return (void*)CurrentPointer;
}

bool MemoryBuffer::operator==(const char* ToCompare)
{
	if ( ToCompare == CurrentPointer )
	{
		return true;
	}
	return false;
}

bool MemoryBuffer::operator==(const void* ToCompare)
{
	if ( ToCompare == (void*)CurrentPointer )
	{
		return true;
	}
	return false;
}

bool MemoryBuffer::operator==(const MemoryBuffer& ToCompare)
{
	if ( ToCompare.CurrentPointer == CurrentPointer )
	{
		return true;
	}
	return false;
}