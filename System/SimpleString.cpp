#include <System/SimpleString.h>


#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
	#define snprintf _snprintf
#endif

SimpleString::StringData SimpleString::StringData::EmptyStringData("");

SimpleString::StringData* SimpleString::StringData::GetEmptyStringData()
{
	EmptyStringData.AddReference(); 
	return &EmptyStringData;
}

SimpleString::StringData::StringData()
{
	data = NULL; 
	length = 0;
	
	AddReference();
}

SimpleString::StringData::StringData(const char* str)
{
	data = NULL; 
	length = 0;
	
	SetData(str);
	
	AddReference();
}

SimpleString::StringData::StringData(const StringData& to_copy)
{
	data = NULL;
	length = 0;
	
	SetData(to_copy.GetDataPtr());
	
	AddReference();
}

SimpleString::StringData::StringData(const StringData& base, int begin, 
                       int end)
{
	data = NULL;
	length = end-begin;
	data = new char[length + 1];
	memcpy(data, base.GetDataPtr()+begin, length);
	*(data+length) = '\0';
	AddReference();
}

SimpleString::StringData::StringData(const char* str1, const char* str2)
{
	data = NULL;
	length = 0;
	
	int l1 = (int)strlen(str1);
	int l2 = (int)strlen(str2);
	length = l1+l2;
	data = new char[length + 1];
	memcpy(data, str1, l1);
	memcpy(data+l1, str2, l2);
	*(data+length) = '\0';
	
	AddReference();
}

SimpleString::StringData::~StringData()
{
	if (data) delete data;
	data = NULL;
}

void SimpleString::StringData::SetData(const char* str)
{
	if (data) delete data;
	
	if(str){
		data = strdup(str);
		length = (unsigned int)strlen(data);
	}else{
		data = NULL;
		length = 0;
	}
}

bool SimpleString::StringData::ChangeData(const char* str)
{
	if(nbReference == 1)
	{
		if(str == NULL)
		{
			if (data) delete data;
			data = NULL;
			length = 0;
		}
		else
		{
			length = (unsigned int)strlen(str);
			if ( data )
			{
				delete data;
			}
			data = new char[length+1];
			if ( data == NULL )
			{
				length = 0;
			}
			else
			{
				strcpy(data, str);
			}
#if 0
			if ( data == NULL )
			{
				data = new char[length+1];
				if ( data == NULL )
				{
					length = 0;
				}
				else
				{
					strcpy(data, str);
				}
			}
			else
			{

				char * datatmp = (char *)realloc(data, length+1);
				if ( datatmp == NULL )
				{
					delete data;
					data = NULL;
					length = 0;
				}
				else
				{
					data = datatmp;
					strcpy(data, str);
				}
			}
#endif
		}
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////

const SimpleString SimpleString::EmptyString("");

SimpleString::SimpleString()
{ 
	stringData = StringData::GetEmptyStringData();
}

SimpleString::SimpleString(const char* str)
{ 
	if(str) stringData = new StringData(str);
	else stringData = StringData::GetEmptyStringData();
}

SimpleString::SimpleString(const char* str1, const char* str2)
{ 
	if(str1 == (const char*)NULL && str2 == (const char*)NULL)
		stringData = StringData::GetEmptyStringData();
	else
		stringData = new StringData(str1, str2);
}

SimpleString::SimpleString(const SimpleString& to_copy)
{
	stringData = to_copy.stringData;
	stringData->AddReference();
}

SimpleString::SimpleString(StringData* sd)
{ stringData = sd; }

SimpleString::~SimpleString()
{
	DestroyStringData();
}

void SimpleString::DestroyStringData()
{
	if(stringData->RemoveReference() <= 0)
		delete stringData;
	stringData = NULL;
}


SimpleString& SimpleString::operator= (const SimpleString& str)
{
	DestroyStringData();
	stringData = str.stringData;
	stringData->AddReference();
	return *this;
}

SimpleString& SimpleString::operator= (const char* str)
{
	if(str == NULL){
		DestroyStringData();
		stringData = StringData::GetEmptyStringData();
	}else if(!stringData->ChangeData(str))
	{
		DestroyStringData();
		stringData = new StringData(str);
	}
	return *this;
}


void SimpleString::Append(const char* str)
{
	if(str != NULL){
		StringData* tmp = new StringData(GetStr(), str);
		DestroyStringData();
		stringData = tmp;
	}
}

void SimpleString::Append(const SimpleString& str){
	Append(str.GetStr());
}


char SimpleString::operator[](int i) const
{ return *(stringData->GetDataPtr()+i);}

char& SimpleString::operator[](int i)
{
	if(stringData->GetNbReference() != 1){
		
		StringData* tmp = new StringData(*stringData);
		DestroyStringData();
		stringData = tmp;
	}
	return *(stringData->GetDataPtr()+i);
}

SimpleString& SimpleString::operator+= (const char* str){
	Append(str);
	return *this;
}

SimpleString& SimpleString::operator+= (const SimpleString& str){
	Append(str);
	return *this;
}

SimpleString& SimpleString::operator+= (int i){
	char tmp[20];
	snprintf(tmp, 20, "%d", i);
	Append(tmp);
	return *this;
}

SimpleString& SimpleString::operator+= (long l){
	char tmp[20];
	snprintf(tmp, 20, "%li", l);
	Append(tmp);
	return *this;
}

SimpleString& SimpleString::operator+= (double d){
	char tmp[30];
	snprintf(tmp, 30, "%lf", d);
	Append(tmp);
	return *this;
}

SimpleString SimpleString::SubString(int begin, int end){
	StringData* sd = new StringData(GetStr(), begin, end);
	return SimpleString(sd);
}

SimpleString operator+(const SimpleString& str1, const SimpleString& str2){
	if(str1.GetStr() == NULL) return SimpleString(str2);
	if(str2.GetStr() == NULL) return SimpleString(str1);
	return SimpleString(str1.GetStr(), str2.GetStr());
}

SimpleString operator+(const char* str1, const SimpleString& str2){
	if(str1 == NULL) return SimpleString(str2);
	if(str2.GetStr() == NULL) return SimpleString(str1);
	return SimpleString(str1, str2.GetStr());
}

SimpleString operator+(const SimpleString& str1, const char* str2){
	if(str1.GetStr() == NULL) return SimpleString(str2);
	if(str2 == NULL) return SimpleString(str1);
	return SimpleString(str1.GetStr(), str2);
}

#define TAILLE_BUFFER 64*1024*1024

static char Latin1ToUTF8Buffer[TAILLE_BUFFER]; // 64 Ko, pas mal...

// WARNING : No multithread-support...
const char * SimpleString::Latin1ToUTF8( const char *src )
{
	const char * parse = src;
	int res;

	for( res=0; res < TAILLE_BUFFER; parse++, res++ )
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
			if ( res+1 >= (TAILLE_BUFFER+1) )
			{
				return NULL;
			}

			Latin1ToUTF8Buffer[res]   = (char)0xc3;
			Latin1ToUTF8Buffer[++res] = (char)(*parse-64);
		}
	}

	if ( res >= TAILLE_BUFFER )
		return NULL;

	return Latin1ToUTF8Buffer;
}

#undef TAILLE_BUFFER

#ifdef WIN32
	#undef snprintf
#endif
