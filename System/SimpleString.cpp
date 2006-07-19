#include <System/SimpleString.h>
#include <System/Portage.h>



using namespace Omiscid;

//SimpleString::StringData SimpleString::StringData::EmptyStringData("");
//
//SimpleString::StringData* SimpleString::StringData::GetEmptyStringData()
//{
//	// EmptyStringData.AddReference(); 
//	return &EmptyStringData;
//}

SimpleString::StringData::StringData()
{
	Lock();

	data = NULL; 
	length = 0;
	nbReferences = NULL;

	Unlock();
}

int SimpleString::StringData::AddReference()
{
	int res;

	Lock();
	
	if ( nbReferences == NULL )
	{
		nbReferences = new AtomicReentrantCounter(1);
		res = 1;
	}
	else
	{
		res = ++(*nbReferences);
	}

	Unlock();

	return res;
}

SimpleString::StringData::StringData(const char* str)
{
	Lock();

	data = NULL;
	length = 0;
	nbReferences = NULL;
	
	SetData(str);
	
	// Create a new AtomicCounter for all these buffer
	AddReference(); 

	Unlock();
}

SimpleString::StringData& SimpleString::StringData::operator=(const StringData& Right)
{
	StringData& TheOther = (StringData&)Right;

	Lock();
	TheOther.Lock();

	data = Right.data;
	length = Right.length;
	nbReferences = Right.nbReferences;
	
	// Add references to the shared atomic counter
	AddReference();

	TheOther.Unlock();
	Unlock();

	return (*this);
}

SimpleString::StringData::StringData(const StringData& to_copy)
{
	operator=(to_copy);
}

SimpleString::StringData::StringData(const StringData& base, int begin, int end)
{
	StringData& TheOther = (StringData&)base;

	Lock();
	TheOther.Lock();

	data = NULL;
	nbReferences = NULL;

	length = end-begin;
	data = new char[length + 1];
	memcpy(data, TheOther.GetDataPtr()+begin, length);
	*(data+length) = '\0';

	AddReference();

	TheOther.Unlock();
	Unlock();
}

SimpleString::StringData::StringData(const char* str1, const char* str2)
{
	Lock();

	data = NULL;
	length = 0;
	nbReferences = NULL;
	
	int l1 = (int)strlen(str1);
	int l2 = (int)strlen(str2);
	length = l1+l2;
	data = new char[length + 1];
	memcpy(data, str1, l1);
	memcpy(data+l1, str2, l2);
	*(data+length) = '\0';
	
	AddReference();

	Unlock();
}

SimpleString::StringData::~StringData()
{
	Lock();

	if (data)
	{
		delete data;
	}
	data = NULL;

	if ( nbReferences )
	{
		delete nbReferences;
	}
	nbReferences = NULL;

	Unlock();
}

void SimpleString::StringData::SetData(const char* str)
{
	Lock();

	if ( data )
	{
		delete data;
	}
	
	if ( str )
	{
		// data = strdup(str);
		length = (unsigned int)strlen(str);
		data = new char[length+1];
		memcpy( data, str, length+1 ); // +1 for the '\0'
	}
	else
	{
		data = NULL;
		length = 0;
	}

	Unlock();
}

void SimpleString::StringData::Lock()
{
	Protect.EnterMutex();
}

void SimpleString::StringData::Unlock()
{
	Protect.LeaveMutex();
}

int SimpleString::StringData::RemoveReference()
{
	int res;

	Lock();
	if ( nbReferences == NULL )
	{
		return -1;
	}
	res = --(*nbReferences);
	Unlock();

	return res; 
}

int SimpleString::StringData::GetNbReference()
{
	int res;

	Lock();
	if ( nbReferences == NULL )
	{
		return -1;
	}
	res = *nbReferences;
	Unlock();

	return res;
}

char* SimpleString::StringData::GetDataPtr()
{
	char * res;
	
	Lock();
	res = data;
	Unlock();

	return res; 
}

unsigned int SimpleString::StringData::GetLength()
{
	unsigned int res;

	Lock();
	res = length;
	Unlock();

	return res;
}

bool SimpleString::StringData::Equals(const char* str)
{
	bool res;

	if ( str == NULL )
	{
		return false;
	}
	
	Lock();
	res = (strcmp(str, data) == 0);
	Unlock();

	return res;
}

bool SimpleString::StringData::Equals(const StringData& sd)
{
	bool res;

	StringData& other = (StringData&)sd;

	Lock();
	res = (this == &sd) || Equals(other.GetDataPtr());
	Unlock();

	return res;
}

bool SimpleString::StringData::NotEquals(const char* str)
{
	bool res;

	if ( str == NULL )
	{
		return true;
	}

	Lock();
	res = (strcmp(str, data) != 0);
	Unlock();

	return res;
}

bool SimpleString::StringData::NotEquals(const StringData& sd)
{
	bool res;

	StringData& other = (StringData&)sd;

	Lock();
	res = (this != &sd) && NotEquals(other.GetDataPtr());
	Unlock();

	return res;
}


//bool SimpleString::StringData::ChangeData(const char* str)
//{
//	Lock();
//
//	if ( nbReferences 
//
//	if(*nbReferences == 1)
//	{
//		if(str == NULL)
//		{
//			if (data)
//			{
//				delete data;
//			}
//			data = NULL;
//			length = 0;
//		}
//		else
//		{
//			length = (unsigned int)strlen(str);
//			if ( data )
//			{
//				delete data;
//			}
//			data = new char[length+1];
//			if ( data == NULL )
//			{
//				length = 0;
//			}
//			else
//			{
//				strcpy(data, str);
//			}
//		}
//		Unlock();
//		return true;
//	}
//
//	Unlock();
//	return false;
//}


///////////////////////////////////////////////////////

const SimpleString SimpleString::EmptyString("");

void SimpleString::CopyStringData(StringData* to_copy)
{
	DestroyStringData();
	// Nothing to copy, set an empty string...
	if ( to_copy == NULL )
	{
		stringData = new StringData( "" );
	}
	else
	{
		to_copy->AddReference();
		stringData = to_copy;
	}
}

SimpleString::SimpleString()
{
	stringData = NULL;
	CopyStringData( SimpleString::EmptyString.stringData );
}

SimpleString::SimpleString(const char* str)
{ 
	stringData = NULL;
	if ( str && str[0] != '\0' )
	{
		stringData = new StringData(str);
	}
	else
	{
		CopyStringData( SimpleString::EmptyString.stringData );
	}
}

SimpleString::SimpleString(const char* str1, const char* str2)
{ 
	stringData = NULL;
	if(str1 == (const char*)NULL && str2 == (const char*)NULL)
		CopyStringData( SimpleString::EmptyString.stringData );
	else
		stringData = new StringData(str1, str2);
}

SimpleString::SimpleString(const SimpleString& to_copy)
{
	stringData = NULL;
	CopyStringData(to_copy.stringData);
}

SimpleString::SimpleString(StringData* sd)
{
	stringData = NULL;
	CopyStringData(sd);
}

SimpleString::SimpleString(int i)
{
	stringData = NULL;
	Empty();
	operator+=(i);
}

SimpleString::SimpleString(unsigned int ui)
{
	stringData = NULL;
	Empty();
	operator+=(ui);
}

SimpleString::SimpleString(long int li)
{
	stringData = NULL;
	Empty();
	operator+=(li);
}

SimpleString::SimpleString(float f)
{
	stringData = NULL;
	Empty();
	operator+=(f);
}

SimpleString::SimpleString(double d)
{
	stringData = NULL;
	Empty();
	operator+=(d);
}

SimpleString::~SimpleString()
{
	DestroyStringData();
}

void SimpleString::Empty()
{
	operator=((char*)NULL);
}

void SimpleString::DestroyStringData()
{
	if ( stringData == NULL )
	{
		return;
	}

	if ( stringData->RemoveReference() <= 0 )
	{
        delete stringData;
	}

	stringData = NULL;
}

const SimpleString& SimpleString::operator= (const SimpleString& str)
{
	DestroyStringData();
	CopyStringData( str.stringData );
	return *this;
}

const SimpleString& SimpleString::operator= (const char* str)
{
	DestroyStringData();
	if(str == NULL)
	{
		CopyStringData( SimpleString::EmptyString.stringData );
	}
	else
	{
		stringData = new StringData(str);
	}
	return *this;
}

const SimpleString& SimpleString::operator= (int i)
{
	Empty();
	return operator+=(i);
}

const SimpleString& SimpleString::operator= (unsigned int ui)
{
	Empty();
	return operator+=(ui);
}

const SimpleString& SimpleString::operator= (long int li)
{
	Empty();
	return operator+=(li);
}

const SimpleString& SimpleString::operator= (float f)
{
	Empty();
	return operator+=(f);
}

const SimpleString& SimpleString::operator= (double d)
{
	Empty();
	return operator+=(d);
}

void SimpleString::Append(const char* str)
{
	if(str != NULL)
	{
		StringData* tmp = new StringData(GetStr(), str);
		DestroyStringData();
		stringData = tmp;
	}
}

void SimpleString::Append(const SimpleString& str)
{
	Append(str.GetStr());
}

char SimpleString::operator[](int i) const
{
	if ( i >= (int)stringData->GetLength() )
	{
		throw "SimpleString::out of bound";
	}
	return *(stringData->GetDataPtr()+i);
}

char& SimpleString::operator[](int i)
{
	if(stringData->GetNbReference() != 1)
	{
		StringData* tmp = new StringData(*stringData);
		DestroyStringData();
		stringData = tmp;
	}
	return *(stringData->GetDataPtr()+i);
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
	StringData* sd = new StringData(GetStr(), begin, end);
	return SimpleString(sd);
}

const SimpleString Omiscid::operator+(const SimpleString& str1, const SimpleString& str2)
{
	if(str1.GetStr() == NULL) return SimpleString(str2);
	if(str2.GetStr() == NULL) return SimpleString(str1);
	return SimpleString(str1.GetStr(), str2.GetStr());
}

const SimpleString Omiscid::operator+(const char* str1, const SimpleString& str2)
{
	if(str1 == NULL) return SimpleString(str2);
	if(str2.GetStr() == NULL) return SimpleString(str1);
	return SimpleString(str1, str2.GetStr());
}

const SimpleString Omiscid::operator+(const SimpleString& str1, const char* str2)
{
	if(str1.GetStr() == NULL) return SimpleString(str2);
	if(str2 == NULL) return SimpleString(str1);
	return SimpleString(str1.GetStr(), str2);
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
	return (const char*)stringData->GetDataPtr(); 
}

unsigned int SimpleString::GetLength() const
{
	return stringData->GetLength(); 
}

bool SimpleString::IsEmpty() const
{
	return (stringData == NULL || stringData->GetLength() == 0 );
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

#ifdef WIN32
	#undef snprintf
#endif
