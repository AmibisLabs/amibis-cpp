#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>
#include <System/AtomicCounter.h>

class MemoryBuffer
{
private:
		/**
	 * \brief Storage of characters, with a counter of reference.
	 */
	class InternalMemoryBuffer
	{
	public:
		/** @name Constructors 
		 *
		 * Add automatically one reference on the object
		 */
		//@{
		InternalMemoryBuffer();
		InternalMemoryBuffer(const InternalMemoryBuffer&);
		//@}
		
		/** @brief Destructor
		 *
		 * free the buffer of character allocated for the string
		 */
		~InternalMemoryBuffer();

		/** \return the pointer on the buffer of characters */
		char* GetDataPtr() const;

		/** \return the length of the string */
		unsigned int GetLength() const;

		/** \brief add one reference to the string 
		 * \return the number of reference
		 */
		int AddReference();

		/** \brief remove a reference on the string 
		 * \return the number of reference existing yet
		 */
		int RemoveReference();

		/** \return the number of reference on the string */
		int GetNbReference();
		
		/** \brief Change the string contained by the object if possible
		 *
		 * The string is changed if only a reference exist on this object.
		 * \return true if the string has changed.
		 */
		bool ChangeData(const char* str);

		/** \name Comparaison */
		//@{
		/** \return if the string contained by this object match the string contained in 'sd'*/
		bool Equals(const InternalMemoryBuffer& sd) const;

		/** \return if the string contained by this object match the string 'str'*/
		bool Equals(const char* str) const;	

		/** \return if the string contained by this object do not match the string contained in 'sd'*/
		bool NotEquals(const InternalMemoryBuffer& sd) const;

		/** \return if the string contained by this object do not match the string 'str''*/
		bool NotEquals(const char* str) const;	
		//@}

	private:	
		AtomicCounter NbReference; /*!< number of reference on the buffer */
		char * Buffer; /*!< the character buffer */
		unsigned int Length;/*!< the length of the string */
	};

public:
	MemoryBuffer();
	MemoryBuffer(MemoryBuffer&copy);
	~MemoryBuffer();

	char operator[](int Pos);

	const MemoryBuffer operator+(int Pos);
	const MemoryBuffer operator+=(int Pos);

	bool operator==(const char* ToCompare);
	bool operator==(const void* ToCompare);
	bool operator==(const MemoryBuffer&ToCompare);

	operator char*();
	operator const char *();
	operator void*();
	operator const void*();

protected:
	InternalMemoryBuffer * OriginalAlloc;
	unsigned int Shift;
};

#endif