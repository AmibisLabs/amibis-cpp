/**
 * @file SimpleString.h
 * @brief Definition of SimpleString class
 */
#ifndef __SIMPLE_STRING_H__
#define __SIMPLE_STRING_H__

#include <System/Config.h>
#include <System/AtomicReentrantMutex.h>

#include <string.h>

namespace Omiscid {

/**
 * @class SimpleString  SimpleString.h  System/SimpleString.h
 * @brief Storage and manipulation of character string.
 *
 * The character string are duplicate only when they are modified.
 * The copy are done only if necessary. 
 * (Use of a counter of reference on the character string)
 *
 * By default, strings are equals to "".
 * @author Sebastien Pesnel
 * @author Dominique Vaufreydaz
 */
class SimpleString
{
private:
	/**
	 * \brief Storage of characters, with a counter of reference.
	 */
	class StringData
	{
	public:
		/** @name Constructors 
		 *
		 * Add automatically one reference on the object
		 */
		//@{
		StringData();
		StringData(const char*);
		StringData(const StringData&);
		StringData(const StringData&, int begin, int end);
		StringData(const char* str1, const char* str2);
		//@}
		
		/** @brief Destructor
		 *
		 * free the buffer of character allocated for the string
		 */
		~StringData();

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
		bool Equals(const StringData& sd) const;
		/** \return if the string contained by this object match the string 'str'*/
		bool Equals(const char* str) const;	
		/** \return if the string contained by this object do not match the string contained in 'sd'*/
		bool NotEquals(const StringData& sd) const;
		/** \return if the string contained by this object do not match the string 'str''*/
		bool NotEquals(const char* str) const;	
		//@}

		/**
		 * return an object StringData containing an empty string : "".
		 * Add a reference on this object.
		 */
		static StringData* GetEmptyStringData();

		void Lock(); // Lock my Protect mutex
		void Unlock(); // Unlock my Protect mutex

		StringData& operator=(const StringData& Right);

	private:	
		/** \brief set the buffer value 
		 *
		 * allocate and free the buffer if necessary.
		 * \param b [in] the buffer to copy
		 */
		void SetData(const char* b);

		// Protect acces to the internal members
		ReentrantMutex Protect;

	protected:
		AtomicReentrantMutex * nbReferences; /*!< number of reference on the buffer */
		char * data; /*!< the character buffer */
		unsigned int length;/*!< the length of the string */
		static StringData EmptyStringData; /*!< object StringData for empty string "" */
	};

public:
	/** @name Constructors */
	//@{
	SimpleString(); /*!< build an empty string */
	SimpleString(const char* str); /*!< build a string with the value 'str'*/
	/*! Build a String with the value 'str1' followed by 'str2'*/
	SimpleString(const char* str1, const char* str2);
	/*! Copy the the string */
	SimpleString(const SimpleString& to_copy);
	//@}
	
	/** @brief Destructor */
	~SimpleString();

	//* @brief To empty a string */
	void Empty();

	/** @name Data Access */
	//@{
	/** @return the character string */
	const char* GetStr() const;
	/** @return the length of the string */
	unsigned int GetLength() const;

	/** @return true if the string is empty, false otherwise */
	bool IsEmpty() const;

	/** read access on the characters
	 * @param i [in] the index of the character between 0 and GetLength()-1
	 */
	char operator[](int i) const;
	/** read-write access on the characters
	 * @param i [in] the index of the character between 0 and GetLength()-1
	 */
	char& operator[](int i);
	//@}
	
	/** @name Data Manipulation : Append String */
	//@{
	/*! add 'str' to the end of this string */
	void Append(const char* str);
	/*! add 'str' to the end of this string */
	void Append(const SimpleString& str);
	/*! add 'str' to the end of this string */
	SimpleString& operator+= (const char* str);
	/*! add 'str' to the end of this string */
	SimpleString& operator+= (const SimpleString& str);
	/*! add the integer 'i' changed in string to the end of this string */
	SimpleString& operator+= (unsigned int ui);
	/*! add the integer 'i' changed in string to the end of this string */
	SimpleString& operator+= (int i);
	/*! add the long integer 'l' changed in string to the end of this string */
	SimpleString& operator+= (long l);
	/*! add the long integer 'l' changed in string to the end of this string */
	SimpleString& operator+= (float f);
	/*! add the integer 'd' changed in string (using no specific formating) to the end of this string */
	SimpleString& operator+= (double d);
	//@}
	
	/** @name Data Comparaison */
	//@{
	bool operator==(const SimpleString& str) const;
	bool operator==(const char* str) const;
	bool operator!=(const SimpleString& str) const;
	bool operator!=(const char* str) const;
	//@}

	/** @name Affectation */
	//@{
	const SimpleString& operator= (const char* str);
	const SimpleString& operator= (const SimpleString& str);
	const SimpleString& operator= (int i);
	const SimpleString& operator= (unsigned int ui);
	const SimpleString& operator= (long int li);
	const SimpleString& operator= (float f);
	const SimpleString& operator= (double d);
	//@}
	
	static bool Latin1ToUTF8( const char *Src, char * Latin1ToUTF8Buffer, int SizeOfBuffer );

	// REVIEW
	static const SimpleString EmptyString;

	/** @brief Extract a string of  this string
	 * \param begin index of the first character included in the result string
	 * \param end index of the first excluded character
	 */
	SimpleString SubString(int begin, int end) const;

protected:
	SimpleString(StringData*); /*!< used by SubString*/
	
private:
	/** \brief Remove the reference on the StringData object.
	 *
	 * Delete the StringData object, if the number of reference becomes 0.
	 */
	void DestroyStringData();

	void CopyStringData(StringData* to_copy);

	StringData* stringData; /*!< pointer on the object containing the characters*/
};


SimpleString operator+(const SimpleString& str1, const SimpleString& str2);
SimpleString operator+(const char* str1, const SimpleString& str2);
SimpleString operator+(const SimpleString& str1, const char* str2);

} // namespace Omiscid

#endif // __SIMPLE_STRING_H__
