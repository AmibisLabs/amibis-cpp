/**
 * @file SimpleString.h
 * @ingroup System
 * @ingroup UserFriendly
 * @brief Definition of SimpleString class
 */

#ifndef __SIMPLE_STRING_H__
#define __SIMPLE_STRING_H__

#include <System/Config.h>
#include <System/AtomicReentrantCounter.h>

#include <string.h>

#include <iostream>

namespace Omiscid {

/**
 * @class SimpleString  SimpleString.h  System/SimpleString.h
 * @ingroup System
 * @ingroup UserFriendly
 * @brief Storage and manipulation of character strings.
 *
 * The character strings are duplicate only when they are modified.
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
	 * @brief Storage of characters, with a counter of reference.
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
		/* virtual */ ~StringData();

		/** \return the pointer on the buffer of characters */
		char* GetDataPtr();

		/** \return the length of the string */
		unsigned int GetLength();

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
		// bool ChangeData(const char* str);

		/** \name Comparaison */
		//@{
		/** \return if the string contained by this object match the string contained in 'sd'*/
		bool Equals(const StringData& sd);
		/** \return if the string contained by this object match the string 'str'*/
		bool Equals(const char* str);	
		/** \return if the string contained by this object match the string contained in 'sd'*/
		bool EqualsCaseInsensitive(const StringData& sd);
		/** \return if the string contained by this object match the string 'str'*/
		bool EqualsCaseInsensitive(const char* str);	
		/** \return if the string contained by this object do not match the string contained in 'sd'*/
		bool NotEquals(const StringData& sd);
		/** \return if the string contained by this object do not match the string 'str''*/
		bool NotEquals(const char* str);	
		/** \return if the string contained by this object do not match the string contained in 'sd'*/
		bool NotEqualsCaseInsensitive(const StringData& sd);
		/** \return if the string contained by this object do not match the string 'str''*/
		bool NotEqualsCaseInsensitive(const char* str);	
		//@}

		/**
		 * return an object StringData containing an empty string : "".
		 * Add a reference on this object.
		 */
		// static StringData* GetEmptyStringData();

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
		AtomicReentrantCounter * nbReferences; /*!< number of reference on the buffer */
		char * data; /*!< the character buffer */
		unsigned int length;/*!< the length of the string */
		// static StringData EmptyStringData; /*!< object StringData for empty string "" */
	};

public:
	SimpleString(); /*!< build an empty string */
	SimpleString(const char* str); /*!< build a string with the value 'str'*/
	/*! Build a String with the value 'str1' followed by 'str2'*/
	SimpleString(const char* str1, const char* str2);
	/*! Copy constructor of the string */
	SimpleString(const SimpleString& to_copy);
	/*! Creating a string representing an interger */
	SimpleString(int i);
	/*! Creating a string representing an unsigned interger */
	SimpleString(unsigned int ui);
	/*! Creating a string representing a long interger */
	SimpleString(long int li);
	/*! Creating a string representing a float*/
	SimpleString(float f);
	/*! Creating a string representing a double */
	SimpleString(double d);
protected:
	SimpleString(StringData*); /*!< used by SubString*/

public:
	/*! Destructor (free if necessary, the underlying buffer) */
	virtual ~SimpleString(); 

	/**
	 * @brief To empty a string
	 */
	void Empty();

	/** @name Data Access */
	//@{
	/** @return the character string */
	const char* GetStr() const;
	/** @return the length of the string */
	unsigned int GetLength() const;

	/** read access on the characters
	 * @param i [in] the index of the character between 0 and GetLength()-1
	 */
	char operator[](int i) const;
	/** read-write access on the characters
	 * @param i [in] the index of the character between 0 and GetLength()-1
	 */
	char& operator[](int i);
	//@}
	
	/**
	 * @return true if the string is empty, false otherwise
	 */
	bool IsEmpty() const;

	/** @name Affectation */
	//@{
	/*! assign 'str' as content for this string */
	const SimpleString& operator= (const char* str);
	/*! assign 'str' as content for this string */
	const SimpleString& operator= (const SimpleString& str);
	/*! assign a string representation of an interger as content for this string */
	const SimpleString& operator= (int i);
	/*! assign a string representation of an unsigned interger as content for this string */
	const SimpleString& operator= (unsigned int ui);
	/*! assign a string representation of a long interger as content for this string */
	const SimpleString& operator= (long int li);
	/*! assign a string representation of a float as content for this string */
	const SimpleString& operator= (float f);
	/*! assign a string representation of double as content for this string */
	const SimpleString& operator= (double d);
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
	/*! Compare (case sensitive) 2 strings to check if they are equals */
	bool operator==(const SimpleString& str) const;
	/*! Compare (case sensitive) 2 strings to check if they are equals */
	bool operator==(const char* str) const;
	/*! Compare (case sensitive) 2 strings to check if they are *not* equals */
	bool operator!=(const SimpleString& str) const;
	/*! Compare (case sensitive) 2 strings to check if they are *not* equals */
	bool operator!=(const char* str) const;
	/*! Compare (case insensitive) 2 strings to check if they are equals */
	bool EqualsCaseInsensitive(const SimpleString& str) const;
	/*! Compare (case insensitive) 2 strings to check if they are equals */
	bool EqualsCaseInsensitive(const char * str) const;
	// to do
	// bool CompareNoCase(const char* str) const;
	// bool CompareNoCase(const SimpleString& str) const;
	//@}
	
	static bool Latin1ToUTF8( const char *Src, char * Latin1ToUTF8Buffer, int SizeOfBuffer );

	/*! All empty string refer to this unique value, can be used by user */
	static const SimpleString EmptyString;

	/** @brief Extract a string of  this string
	 * @param [in] begin index of the first character included in the result string
	 * @param [in] end index of the first excluded character
	 * return A new string conaining the sub-string
	 */
	SimpleString SubString(int begin, int end) const;

	int Find(const SimpleString SearchPattern, bool Backward = false) const;

	bool ReplaceFirst(const SimpleString SearchPattern, const SimpleString ReplacedPattern);
	bool ReplaceAll(const SimpleString SearchPattern, const SimpleString ReplacedPattern);

private:
	/** \brief Remove the reference on the StringData object.
	 *
	 * Delete the StringData object, if the number of reference becomes 0.
	 */
	void DestroyStringData();

	void CopyStringData(StringData* to_copy);

	StringData* stringData; /*!< pointer on the object containing the characters*/
};

/*
 * @brief An operator+ to collapse 2 strings
 * @param [in] str1, a SimpleString
 * @param [in] str2, a SimpleString
 * @return Returns a SimpleString
 */
const SimpleString operator+(const SimpleString& str1, const SimpleString& str2);
/*
 * @brief An operator+ to collapse a char buffer and a string string
 * @param [in] str1, a char * buffer (ended by '0')
 * @param [in] str2, a SimpleString
 * @return Returns a SimpleString
 */

const SimpleString operator+(const char* str1, const SimpleString& str2);

/*
 * @brief An operator+ to collapse a char buffer and a string string
 * @param [in] str1, a SimpleString
 * @param [in] str2, a char * buffer (ended by '0')
 * @return Returns a SimpleString
 */
const SimpleString operator+(const SimpleString& str1, const char* str2);

/*
 * @brief A serialisation input operator
 * @param [in] is, an input stream
 * @param [in] str2, a char * buffer (ended by '0')
 * @return Returns the input stream
 */
std::istream& operator>>( std::istream &is , SimpleString &str );

/*
 * @brief A serialisation input operator
 * @param [in] is, an input stream
 * @param [in] str2, a char * buffer (ended by '0')
 * @return Returns the input stream
 */
std::ostream& operator<<( std::ostream &os, const SimpleString &str );

} // namespace Omiscid

#endif // __SIMPLE_STRING_H__
