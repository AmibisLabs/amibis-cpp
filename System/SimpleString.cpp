#include <System/SimpleString.h>

#include <System/LockManagement.h>
#include <System/Portage.h>

#include <string.h>


using namespace Omiscid;

// Empty SimpleString
const SimpleString SimpleString::EmptyString;

SimpleString::StringData::StringData()
{
	data = GetEmptyBuffer();
	length = 0;
}

SimpleString::StringData::StringData(const char* str)
{
	data = NULL;	// in order to call SetData
	length = 0;

	SetData(str);
}

SimpleString::StringData::StringData(const StringData* base)
{
	length = base->length;
	data = new OMISCID_TLM char[length + 1];
	memcpy(data, base->data, length);
	*(data+length) = '\0';
}

SimpleString::StringData::StringData(const StringData* base, int begin, int end)
{
	int tmplen = end-begin;
	if ( base == (const StringData*)NULL || tmplen < 0 || (tmplen+begin) > (int)base->length  )
	{
#ifdef DEBUG
		TemporaryMemoryBuffer TmpS(1024);
		sprintf( (char*)TmpS, "Bad parameter for SimpleString::StringData::StringData (%s)\nbase=%p\nbase->length=%d\nbegin=%d\nend=%d\ntmplen=%d\n", base->data, (void*)base, (int)base->length, begin, end, tmplen );
		throw SimpleException( (char*)TmpS );
#else
		throw SimpleException( "Bad parameter for SimpleString::StringData::StringData" );
#endif // DEBUG
	}

	if ( tmplen == 0 )
	{
		data = GetEmptyBuffer();
		length = 0;
	}
	else
	{
		length = end-begin;
		data = new OMISCID_TLM char[length + 1];
		memcpy(data, base->data+begin, length);
		*(data+length) = '\0';
	}
}

SimpleString::StringData::StringData(const char* str1, const char* str2)
{
	int l1 = (int)strlen(str1);
	int l2 = (int)strlen(str2);
	length = l1+l2;
	data = new OMISCID_TLM char[length + 1];
	memcpy(data, str1, l1);
	memcpy(data+l1, str2, l2);
	*(data+length) = '\0';
}

SimpleString::StringData::StringData(const StringData* StringData1, const char* str2)
{
	int l2 = (int)strlen(str2);
	length = StringData1->length + l2;
	data = new OMISCID_TLM char[length + 1];
	memcpy(data, StringData1->data, StringData1->length);
	memcpy(data+StringData1->length, str2, l2);
	*(data+length) = '\0';
}

SimpleString::StringData::StringData(const StringData* StringData1, const StringData* StringData2)
{
	length = StringData1->length + StringData2->length;
	data = new OMISCID_TLM char[length + 1];
	memcpy(data, StringData1->data, StringData1->length);
	memcpy(data+StringData1->length, StringData2->data, StringData2->length);
	*(data+length) = '\0';
}

SimpleString::StringData::~StringData()
{
	if ( data != (char*)NULL )
	{
		delete [] data;
	}
	data = NULL;
}

void SimpleString::StringData::SetData(const char* str)
{
	if ( data != (char*)NULL )
	{
		delete [] data;
	}

	if ( str != (const char*)NULL )
	{
		length = (unsigned int)strlen(str);
		data = new OMISCID_TLM char[length+1];
		memcpy( data, str, length+1 ); // +1 for the '\0'
	}
	else
	{
		data = GetEmptyBuffer();
		length = 0;
	}
}

bool SimpleString::StringData::Equals(const char* str)
{
	if ( str == (const char*)NULL )
	{
		if ( length == 0 )
		{
			// Both are empty, so they are equals
			return true;
		}
		return false;
	}

	return (strcmp(str, data) == 0);
}

bool SimpleString::StringData::Equals(const StringData& sd)
{
	return ((this == &sd) || Equals(sd.data));
}

bool SimpleString::StringData::EqualsCaseInsensitive(const char* str)
{
	if ( str == (const char*)NULL )
	{
		if ( data == (char*)NULL )
		{
			// Both are NULL, so they are equals
			return true;
		}
		return false;
	}

	return (strcasecmp(str, data) == 0);
}

bool SimpleString::StringData::EqualsCaseInsensitive(const StringData& sd)
{
	return ((this == &sd) || EqualsCaseInsensitive(sd.data));
}

bool SimpleString::StringData::NotEquals(const char* str)
{
	if ( str == (const char*)NULL )
	{
		if ( data == (char*)NULL )
		{
			// Both are NULL, so they are equals
			return true;
		}
		return false;
	}

	return (strcmp(str, data) != 0);
}

bool SimpleString::StringData::NotEquals(const StringData& sd)
{
	return ((this != &sd) && NotEquals(sd.data));
}

bool SimpleString::StringData::NotEqualsCaseInsensitive(const char* str)
{
	if ( str == (const char*)NULL )
	{
		if ( data == (char*)NULL )
		{
			// Both are NULL, so they are equals
			return true;
		}
		return false;
	}

	return (strcasecmp(str, data) != 0);
}

bool SimpleString::StringData::NotEqualsCaseInsensitive(const StringData& sd)
{
	return ((this != &sd) && NotEquals(sd.data));
}



///////////////////////////////////////////////////////

/** Lock my internal data */
bool SimpleString::Lock() const
{
	return ((Omiscid::ReentrantMutex&)Protect).Lock();
}

/** Unlock my internal data */
bool SimpleString::Unlock() const
{
	return ((Omiscid::ReentrantMutex&)Protect).Unlock();
}

SimpleString::SimpleString()
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = new OMISCID_TLM StringData();

	Unlock();
}

SimpleString::SimpleString(const char* str)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = new OMISCID_TLM StringData(str);

	Unlock();
}

SimpleString::SimpleString(const char* str1, const char* str2)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = new OMISCID_TLM StringData(str1, str2);

	Unlock();
}

SimpleString::SimpleString(const SimpleString& to_copy)
	: stringData((StringData*)NULL), Protect()
{
	Lock();
	to_copy.Lock();

	// Set data as usual
	// stringData = to_copy.stringData;
	// incr reference count
	// to_copy.stringData->AddReference();

	// Set data as usual
	stringData = new OMISCID_TLM StringData(to_copy.stringData);

	to_copy.Unlock();
	Unlock();
}

/*! Copy constructor for 2 strings */
SimpleString::SimpleString(const SimpleString& to_copy1, const SimpleString& to_copy2)
	: stringData((StringData*)NULL), Protect()
{
	Lock();
	to_copy1.Lock();
	to_copy2.Lock();

	stringData = new OMISCID_TLM StringData((StringData*)to_copy1.stringData,(StringData*)to_copy2.stringData);

	to_copy2.Unlock();
	to_copy1.Unlock();
	Unlock();
}

SimpleString::SimpleString(StringData* sd)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	// Set data as usual
	// stringData = sd;
	// incr reference count
	// sd->AddReference();

	// Set data as usual
	stringData = new OMISCID_TLM StringData(sd);

	Unlock();
}

SimpleString::SimpleString(int i)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = (StringData*)NULL;
	operator+=(i);

	Unlock();
}

SimpleString::SimpleString(unsigned int ui)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = (StringData*)NULL;
	operator+=(ui);

	Unlock();
}

SimpleString::SimpleString(long int li)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = (StringData*)NULL;
	operator+=(li);

	Unlock();
}

SimpleString::SimpleString(float f)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = (StringData*)NULL;
	operator+=(f);

	Unlock();
}

SimpleString::SimpleString(double d)
	: stringData((StringData*)NULL), Protect()
{
	Lock();

	stringData = (StringData*)NULL;
	operator+=(d);

	Unlock();
}

SimpleString::~SimpleString()
{
	// Lock is not done in DestroyStringData
	Lock();
	DestroyStringData();
	Unlock();
}

// WARNING DestroyStringData do not call protect
void SimpleString::DestroyStringData()
{
	if ( stringData != (StringData*)NULL )
	{
		delete stringData;
		stringData = (StringData*)NULL;
	}
}

 /** @brief Set a StringData object.
  *
  */
void SimpleString::SetStringData(StringData* to_set)
{
	if ( to_set == NULL )
	{
		// Set me as empty
		*this = EmptyString;
		return;
	}

	// Lock myself
	Lock();

	// First, destroy My data if any
	DestroyStringData();

	// and then just set the string data with incr reference count
	stringData = to_set;

	Unlock();
}

void SimpleString::Empty()
{
	Lock();

	DestroyStringData();
	stringData = new OMISCID_TLM StringData();

	Unlock();
}

const SimpleString& SimpleString::operator= (const SimpleString& str)
{
	Lock();
	str.Lock();

	DestroyStringData();
	// stringData = str.stringData;
	// stringData->AddReference();
	stringData = new OMISCID_TLM StringData(str.stringData);

	str.Unlock();
	Unlock();

	return *this;
}

const SimpleString& SimpleString::operator= (const char* str)
{
	Lock();

	DestroyStringData();
	stringData = new OMISCID_TLM StringData(str);

	Unlock();

	return *this;
}

const SimpleString& SimpleString::operator= (int i)
{
	Lock();

	DestroyStringData();

	TemporaryMemoryBuffer tmp(25);
	snprintf((char*)tmp, 20, "%d", i);
	stringData = new OMISCID_TLM StringData((char*)tmp);

	Unlock();
	return *this;
}

const SimpleString& SimpleString::operator= (unsigned int ui)
{
	Lock();

	DestroyStringData();

	TemporaryMemoryBuffer tmp(25);
	snprintf((char*)tmp, 20, "%u", ui);

	stringData = new OMISCID_TLM StringData((char*)tmp);

	Unlock();

	return *this;
}

const SimpleString& SimpleString::operator= (long int li)
{
	Lock();

	DestroyStringData();

	TemporaryMemoryBuffer tmp(55);
	snprintf((char*)tmp, 50, "%li", li);
	stringData = new OMISCID_TLM StringData((char*)tmp);

	Unlock();

	return *this;
}

const SimpleString& SimpleString::operator= (float f)
{
	Lock();

	DestroyStringData();

	TemporaryMemoryBuffer tmp(55);
	snprintf((char*)tmp, 50, "%f", f);
	stringData = new OMISCID_TLM StringData((char*)tmp);

	Unlock();

	return *this;
}

const SimpleString& SimpleString::operator= (double d)
{
	Lock();

	DestroyStringData();

	TemporaryMemoryBuffer tmp(55);
	// warning: ISO C++ does not support the '%lf' printf format
	snprintf((char*)tmp, 50, "%f", d);
	stringData = new OMISCID_TLM StringData((char*)tmp);

	Unlock();

	return *this;
}

void SimpleString::Append(const char* str)
{
	if ( str != NULL )
	{
		Lock();

		StringData* tmp;
		if ( stringData != (StringData*)NULL )
		{
			tmp = new OMISCID_TLM StringData(stringData, str);
			DestroyStringData();
			stringData = tmp;
		}
		else
		{
			stringData = new OMISCID_TLM StringData(str);
		}

		Unlock();
	}
}

void SimpleString::Append(const SimpleString& str)
{
	if ( str.stringData->length != 0 )
	{
		Lock();
		str.Lock();

		StringData* tmp = new OMISCID_TLM StringData(stringData, str.stringData);
		DestroyStringData();
		stringData = tmp;

		str.Unlock();
		Unlock();
	}
}

char SimpleString::operator[](int i) const
{
	Lock();

	char tmp;

	if ( i < 0 || i >= (int)stringData->length )
	{
		Unlock();
		throw  SimpleException("SimpleString::out of bound");
	}

	tmp = *(stringData->data+i);

	Unlock();

	// retreive character at position
	return tmp;
}

char& SimpleString::operator[](int i)
{
	Lock();

	if ( i < 0 || i >= (int)stringData->length )
	{
		Unlock();
		throw  SimpleException("SimpleString::out of bound");
	}

	char& tmp = *(stringData->data+i);

	Unlock();

	return tmp;
}

SimpleString& SimpleString::operator+= (const char* str)
{
	Append(str);
	return *this;
}

SimpleString& SimpleString::operator+= (const SimpleString& str)
{
	Append(str);
	return *this;
}

SimpleString& SimpleString::operator+= (int i)
{
	TemporaryMemoryBuffer tmp(20);
	snprintf((char*)tmp, 20, "%d", i);
	Append((char*)tmp);
	return *this;
}

SimpleString& SimpleString::operator+= (unsigned int ui)
{
	TemporaryMemoryBuffer tmp(20);
	snprintf((char*)tmp, 20, "%u", ui);
	Append((char*)tmp);
	return *this;
}

SimpleString& SimpleString::operator+= (long l)
{
	TemporaryMemoryBuffer tmp(50);
	snprintf((char*)tmp, 50, "%li", l);
	Append((char*)tmp);
	return *this;
}

SimpleString& SimpleString::operator+= (float f)
{
	TemporaryMemoryBuffer tmp(50);
	snprintf((char*)tmp, 50, "%f", f);
	Append((char*)tmp);
	return *this;
}

SimpleString& SimpleString::operator+= (double d)
{
	TemporaryMemoryBuffer tmp(50);
	// warning: ISO C++ does not support the '%lf' printf format
	snprintf((char*)tmp, 50, "%f", d);
	Append((char*)tmp);
	return *this;
}

SimpleString SimpleString::SubString(int begin, int end) const
{
	int lend = end;

	if ( lend < 0 )
	{
		if ( lend == -1 )
		{
			lend = GetLength();
		}
		else
		{
			return SimpleString::EmptyString;
		}
	}

	if ( begin >= lend )	// The Could not do the job, or job is empty return an empty string
	{
		return SimpleString::EmptyString;
	}

	// Lock myself
	Lock();

	StringData * sd = new OMISCID_TLM StringData((StringData*)stringData, begin, lend);

	// Unlock
	Unlock();

	if ( sd == NULL )
	{
		return EmptyString;
	}
	else
	{
		SimpleString TmpS;
		TmpS.SetStringData(sd);
		return TmpS;
	}
}

const SimpleString Omiscid::operator+(const SimpleString& str1, const SimpleString& str2)
{
	if ( str1.GetLength() == 0 ) return SimpleString(str2);
	if ( str2.GetLength() == 0 ) return SimpleString(str1);
	return SimpleString(str1, str2);
}

const SimpleString Omiscid::operator+(const char* str1, const SimpleString& str2)
{
	if ( str1 == NULL ) return SimpleString(str2);
	if ( str2.GetLength() == 0 ) return SimpleString(str1);
	return SimpleString( str1, str2 );
}

const SimpleString Omiscid::operator+(const SimpleString& str1, const char* str2)
{
	if ( str1.GetLength() == 0 ) return SimpleString(str2);
	if ( str2 == NULL ) return SimpleString(str1);
	return SimpleString( str1, str2 );
}

int SimpleString::Find(const SimpleString SearchPattern, bool Backward /* = false */ ) const
{
	char * TmpChar;
	char * Buffer;
	int	CurrentLen;

	Lock();
	SearchPattern.Lock();

	if ( SearchPattern.stringData->length == 0 || SearchPattern.stringData->length > stringData->length )
	{
		SearchPattern.Unlock();
		Unlock();
		return -1;
	}

	// Get the current buffer
	Buffer = stringData->data;

	if ( Backward == false )
	{
		// Forward search
		TmpChar = strstr( Buffer, SearchPattern.stringData->data );

		SearchPattern.Unlock();
		Unlock();

		if ( TmpChar == NULL )
		{
			// Not Found
			return -1;
		}
		return (int)(TmpChar-Buffer);
	}
	else
	{
		// backward search from the end of the current string minus the len of the search string
		CurrentLen = stringData->length - SearchPattern.stringData->length;
		for( ; ; )
		{
			TmpChar = (char*)memrchr( (void*)Buffer, (int)SearchPattern[0], (size_t)CurrentLen );
			if ( TmpChar != NULL )
			{
				if ( strncmp( TmpChar, SearchPattern.stringData->data, SearchPattern.stringData->length ) == 0 )
				{
					// ok, found
					SearchPattern.Unlock();
					Unlock();
					return (int)(TmpChar-Buffer);
				}

				// Compute new search len for the next search
				CurrentLen = CurrentLen - (int)(TmpChar-Buffer);
			}
			else
			{
				SearchPattern.Unlock();
				Unlock();
				return -1;
			}
		}
	}
}

bool SimpleString::InternalReplaceFirst(const SimpleString& SearchPattern, const SimpleString& ReplacedPattern)
{
	char * TmpChar;
	char * Buffer;
	SimpleString TmpString;

	if ( stringData->length == 0 )
	{
		// Nothing to find in an empty string
		return false;
	}

	Buffer = stringData->data;

	TmpChar = strstr( Buffer, SearchPattern.stringData->data );
	if ( TmpChar == NULL )
	{
		// Not Found
		return false;
	}

	if ( TmpChar == Buffer )	// Search String is at the beginning of the string
	{
		TmpString = SubString( SearchPattern.stringData->length, stringData->length );
	}
	else
	{
		// Build the string with the remplacement string
		TmpString = SubString(0, (int)(TmpChar-Buffer)) + ReplacedPattern + SimpleString(TmpChar + SearchPattern.stringData->length );
	}

	// Ok, now, I will contain the result
	*this = TmpString;

	return true;
}

bool SimpleString::ReplaceFirst(const SimpleString& SearchPattern, const SimpleString& ReplacedPattern)
{
	Lock();
	SearchPattern.Lock();
	ReplacedPattern.Lock();

	bool ret = InternalReplaceFirst( SearchPattern, ReplacedPattern );

	ReplacedPattern.Unlock();
	SearchPattern.Unlock();
	Unlock();

	return ret;
}

bool SimpleString::ReplaceAll(const SimpleString& SearchPattern, const SimpleString& ReplacedPattern)
{
	bool ret = false;

	Lock();
	SearchPattern.Lock();
	ReplacedPattern.Lock();

	while( InternalReplaceFirst( SearchPattern, ReplacedPattern ) )
	{
		ret = true;
	}

	ReplacedPattern.Unlock();
	SearchPattern.Unlock();
	Unlock();

	return ret;
}



using namespace std;

/*
 * @brief A serialisation input operator
 * @param [in] is, an input stream
 * @param [in] str2, a char * buffer (ended by '0')
 * @return Returns the input stream
 */
istream& Omiscid::operator>>( istream &is , SimpleString &str )
{
	// To optimize !
	char c;
	bool LineIsNotTerminated;

	// Create a new buffer,
	TemporaryMemoryBuffer TmpS( 256 );

	str.Empty();
	// Get the data
	do
	{
		// Get the first 5 characters of the line
		is.get((char*)TmpS,5);
		// Add them to the line
		str += (char*)TmpS;

		is.get( c );

		if ( c == '\n' )
		{
			LineIsNotTerminated = false;
		}
		else
		{
			// push back the character in the input
			is.unget();

			// Loop again
			LineIsNotTerminated = true;
		}
	}
	while ( LineIsNotTerminated == true );

	return is;
}

/*
 * @brief A serialisation input operator
 * @param [in] is, an input stream
 * @param [in] str2, a char * buffer (ended by '0')
 * @return Returns the input stream
 */
std::ostream& Omiscid::operator<<( std::ostream &os, const SimpleString &str )
{
	os << str.GetStr();
	return os;
}

//----------------------------------------------//

const char* SimpleString::GetStr() const
{
	return (const char*)stringData->data;
}

unsigned int SimpleString::GetLength() const
{
	return stringData->length;
}

bool SimpleString::IsEmpty() const
{
	return (stringData == NULL || stringData->length == 0 );
}

bool SimpleString::operator==(const SimpleString& str) const
{
	return stringData->Equals(*(str.stringData));
}

bool SimpleString::operator==(const char* str) const
{
	return stringData->Equals(str);
}

bool SimpleString::operator!=(const SimpleString& str) const
{
	return stringData->NotEquals(*(str.stringData));
}

bool SimpleString::operator!=(const char* str) const
{
	return stringData->NotEquals(str);
}

/*! Compare (case insensitive) 2 strings to check if they are equals */
bool SimpleString::EqualsCaseInsensitive(const SimpleString& str) const
{
	return stringData->EqualsCaseInsensitive(*(str.stringData));
}

/*! Compare (case insensitive) 2 strings to check if they are equals */
bool SimpleString::EqualsCaseInsensitive(const char * str) const
{
	return stringData->EqualsCaseInsensitive(str);
}

bool SimpleString::Latin1ToUTF8( const char *Src, char * Latin1ToUTF8Buffer, int SizeOfBuffer )
{
	const char * parse = Src;
	int res;

	if ( parse == NULL )
		return false;

	for( res=0; res < SizeOfBuffer; parse++, res++ )
	{
		if ( *parse == '\0' )
		{
			Latin1ToUTF8Buffer[res] = '\0';
			break;
		}
		if ( (*parse & 0x80) == 0 ) // ASCII 7
		{
			Latin1ToUTF8Buffer[res] = *parse;
			continue;
		}
		else
		{
			if ( res+1 >= (SizeOfBuffer+1) )
			{
				return false;
			}

			Latin1ToUTF8Buffer[res]   = (char)0xc3;
			Latin1ToUTF8Buffer[++res] = (char)(*parse-64);
		}
	}

	if ( res >= SizeOfBuffer )
		return false;

	return true;
}

#ifdef OMISCID_RUNING_TEST

#include <System/ElapsedTime.h>
#include <System/Event.h>
#include <System/Socket.h>

int SimpleString::TestFunction()
{
	// Test of SimpleString...

	SimpleString Test("metis.local.");

	bool Modif = false;
	Test = Socket::RemoveLocalDomain( Test, Modif );

	SimpleString Tutu;

	Tutu = "\\032Yop";
	OmiscidTrace( "%s\n", Tutu.GetStr() );
	Tutu.ReplaceFirst( "\\032", " " );
	OmiscidTrace( "%s\n", Tutu.GetStr() );

	Tutu = "Yop\\032";
	OmiscidTrace( "%s\n", Tutu.GetStr() );
	Tutu.ReplaceFirst( "\\032", " " );
	OmiscidTrace( "%s\n", Tutu.GetStr() );

	Tutu = "Yop\\032blib";
	OmiscidTrace( "%s\n", Tutu.GetStr() );
	Tutu.ReplaceFirst( "\\032", " " );
	OmiscidTrace( "%s\n", Tutu.GetStr() );

	Tutu = "Yop\\0 liajezijclz jl ijzzlij blib";
	OmiscidTrace( "%s\n", Tutu.GetStr() );
	Tutu.ReplaceFirst( "\\032", " " );
	OmiscidTrace( "%s\n", Tutu.GetStr() );

	Tutu = "Yop\\032liajezijclz jl ijzzlij blib";
	OmiscidTrace( "%s\n", Tutu.GetStr() );
	Tutu.ReplaceAll( "\\032", " " );
	OmiscidTrace( "%s\n", Tutu.GetStr() );

	Tutu = "Yop\\032liajezijclz jl\\032 ijzzlij blib";
	OmiscidTrace( "%s\n", Tutu.GetStr() );
	Tutu.ReplaceAll( "\\032", "+++" );
	OmiscidTrace( "%s\n", Tutu.GetStr() );

	return 0;
}
#endif // OMISCID_RUNING_TEST

#ifdef WIN32
	#undef snprintf
#endif

