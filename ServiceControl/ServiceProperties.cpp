/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/ServiceProperties.h>

#ifdef WIN32
#pragma warning(disable : 4312)
#endif	// WIN32

using namespace Omiscid;

/*! \brief Empty a property.
 *
 *	This function empty a property. The #Length is set to 1 (just the byte containing
 *	the length of the property), the intinsect #BinaryLength of the property (the only byte) is set to 0
 *	as the #NameLength and the #ValueLength.
 *
 *	In debug mode, we also full the Hash with 0.
 */
void ServiceProperty::Empty()
{
	Length = 1;
	BinaryLength = 0;
	NameLength = 0;
	ValueLength = 0;

#ifdef DEBUG
	memset( &Hash[1], 0, sizeof(Hash)-1 );
#endif
}

/*! \brief Default constructor.
 *
 *	The default constructor constructs an empty property (see ServiceProperty#Empty).
 */
ServiceProperty::ServiceProperty() : BinaryLength( (*(unsigned char*)Hash) )
{
	// Just call the empty function
	Empty();
}

/*! \brief Alternate constructor.
 *	\param[in] Name a const c-SimpleString containing the property name.
 *	\param[in] Value an optional const c-SimpleString containing the value of the property
 *	\throw ServicePropertiesException The value of the error message sould be "Bad parameter(s) for the ServiceProperty constructor"
 *
 *	This constructor takes a name and optionaly a value for the new constructed property. See ServiceProperty#SetProperty.
 */
ServiceProperty::ServiceProperty( const char * Name, const char * Value ) : BinaryLength( (*(unsigned char*)Hash) )
{
	// Call the virtual function SetProperty
	if ( SetProperty( Name, Value ) == false )
	{
		// Can not set the name or value for this property. Raise an exception.
		throw ServicePropertiesException( "Bad parameter(s) for the ServiceProperty constructor" );
	}
}

ServiceProperty::~ServiceProperty()
{}

/*! \brief Set name and optionaly value for a property.
 *	\param[in] Name a const c-SimpleString containing the property name.
 *	\param[in] Value an optional const c-SimpleString containing the value of the property
 *  \return a boolean answer if changes are made.
 *
 *	This function changes name and optionaly value for a property. If a problem occurs, the
 *	property will be empty. You can define property as follow:
 *  \li \c if #Name equals "Property" and #Value is NULL, we will have a flag : "Property".
 *  \li \c if #Name equals "Property" and #Value is "", we will have an empty property : "Property=".
 *  \li \c if #Name equals "Property" and #Value is "something", we will have : "Property=something".
 */
bool ServiceProperty::SetProperty( const char * Name, const char * Value )
{
	// First of all, empty the property
	Empty();

	if ( Name == NULL )
	{
		return false;
	}

	size_t tmp_name_length = strlen( Name );
	if ( tmp_name_length > 255 )
	{
	    NameLength = 0;
	    return false;
	}
	else
	{
	    NameLength = (unsigned char)tmp_name_length;	    
	}

	BinaryLength = NameLength;
	strcpy( &Hash[1], Name );

	UpdateProperty( Value );

	return true;
}

bool ServiceProperty::UpdateProperty( const char * Value )
{
	unsigned int tmpLength;

	if ( NameLength == 0 )
	{
		return false;
	}

#ifdef DEBUG
	// In real life, we do not care about padding...
	// Only Lengths attributes (including BinaryLength) are usefull...
	// +1 because of the binarylength at the begining of Hash
	memset( &Hash[NameLength+1], 0, sizeof(Hash)-NameLength);
#endif

	BinaryLength = NameLength;

	if ( Value == NULL )
	{
		Hash[NameLength+1] = '\0';
	}
	else
	{
		// There is a value
		tmpLength = (unsigned int)strlen(Value);
		if ( (tmpLength + (unsigned int)NameLength + (unsigned int)sizeof("=")) > 255 )
		{
			Empty();
			return false;
		}
		// ok, everything is fine
		ValueLength = (unsigned char)tmpLength;
		BinaryLength += sizeof( '=' ) + ValueLength;
		// '\0' is automatically added by sprintf
		sprintf( &Hash[1+NameLength], "=%s", Value );
	}

	Length = BinaryLength + 1;

	return true;
}

const char * ServiceProperty::operator= ( const char * rvalue )
{
	UpdateProperty( rvalue );
	return rvalue;
}

int ServiceProperty::operator= ( int rvalue )
{
	if ( (const char*)rvalue != NULL )
	{
		throw ServicePropertiesException( "Bad r-value : must be NULL (or 0)" );
	}

	UpdateProperty( (const char*)rvalue );
	return rvalue;
}

const char * ServiceProperty::GetValue()
{
	if ( BinaryLength == NameLength )
		return NULL;

	return (const char*)&Hash[2+NameLength];
}

ServiceProperty:: operator char*()
{
	return (char*)GetValue();
}

ServiceProperty:: operator const char*()
{
	return GetValue();
}

ServicePropertyNotify::ServicePropertyNotify() : ServiceProperty(), Container(NULL)
{
}

ServicePropertyNotify::ServicePropertyNotify( const char * Name, const char * Value, ServiceProperties * Parent )
: ServiceProperty( Name, Value ), Container(Parent)
{
}

ServicePropertyNotify::~ServicePropertyNotify()
{}

void ServicePropertyNotify::SetNotify( ServiceProperties * Parent )
{
	Container = Parent;
}

bool ServicePropertyNotify::SetProperty( const char * Name, const char * Value )
{
	bool ret = ServiceProperty::SetProperty( Name, Value );
	if ( Container && ret )
	{
		Container->NotifyChanges();
	}

	return ret;
}


bool ServicePropertyNotify::UpdateProperty( const char * Value )
{
	bool ret = ServiceProperty::UpdateProperty( Value );
	if ( Container && ret )
	{
		Container->NotifyChanges();
	}

	return ret;
}

const char * ServicePropertyNotify::operator= ( const char * rvalue )
{
	const char * ret = ServiceProperty::operator=( rvalue );
	if ( Container && ret )
	{
		Container->NotifyChanges();
	}

	return ret;
}

int ServicePropertyNotify::operator= ( int rvalue )
{
	int ret = ServiceProperty::operator=( rvalue );
	if ( Container && ret )
	{
		Container->NotifyChanges();
	}

	return ret;
}

const ServicePropertyNotify& ServicePropertyNotify::operator= ( const ServicePropertyNotify& rvalue )
{
	ServiceProperty::operator=( rvalue );

	if ( Container )
	{
		Container->NotifyChanges();
	}

	return *this;
}

const ServiceProperty& ServicePropertyNotify::operator= ( const ServiceProperty& rvalue )
{
	ServiceProperty::operator=( rvalue );

	if ( Container )
	{
		Container->NotifyChanges();
	}

	return *this;
}

void ServicePropertyNotify::Copy( const ServicePropertyNotify& rvalue )
{
	// Copy the property itself
	ServiceProperty::Copy( rvalue );

	// Copy the pointer to its container
	Container = rvalue.Container;
}

ServiceProperties::ServiceProperties( int InitialSize )
{
	int i;

	TXTRecordLength = 0;
	NbProperties = 0;
	MaxProperties = 0;
	Properties = NULL;

	if ( InitialSize < 0 || InitialSize > 1024 )	// The smallest field is a flag of 1 car, so 2 bytes
	{
		throw ServicePropertiesException( "Too few or too many properties" );
	}

	// Round size value to the next upper multiple of 2
	i = (InitialSize+1)&~1;

	Properties = new ServicePropertyNotify[i];

	if ( Properties == NULL )
	{
		throw ServicePropertiesException( "Not enought memory" );
	}

	MaxProperties = i;

	for( i = 0; i < MaxProperties; i++ )
	{
		Properties[i].SetNotify( this );
	}
}

ServiceProperties::~ServiceProperties()
{
	if ( Properties )
	{
		delete [] Properties;
	}
}

int ServiceProperties::GetTXTRecordLength()
{
	return TXTRecordLength;
}

ServiceProperty& ServiceProperties::operator[]( const char * Name )
{
	int Pos;
	
	Pos = Find( Name );

	if ( Pos != -1 )
	{
		return Properties[Pos];
	}

	if ( NbProperties == MaxProperties )
	{
		// We must grow, if necessary... We can not be over 1024 properties set...
		if ( (MaxProperties*2) > 1024 )
		{
			throw ServicePropertiesException( "Out of band" );
		}

		// Ok, we will have Twice the current number of Properties
		int i =  MaxProperties * 2;
		
		ServicePropertyNotify * tmpProperties = new ServicePropertyNotify[i];

		if ( tmpProperties == NULL )
		{
			throw ServicePropertiesException( "Not enought memory to grow number of properties" );
		}

		MaxProperties = i;

		// Copy all properties from previous table to new one
		for( i = 0; i < NbProperties; i++ )
		{
			tmpProperties[i].Copy( Properties[i] );
		}

		// Set me as callback in all new properties
		for( /* i = NbProperties */; i < MaxProperties; i++ )
		{
			tmpProperties[i].SetNotify( this );
		}

		// swap tables of properties
		// Free the previous one
		delete [] Properties;

		// Set the new one as the current one
		Properties = tmpProperties;
	}

	Properties[NbProperties].SetProperty( Name, NULL );

	NbProperties++;

	NotifyChanges();

	return Properties[NbProperties-1];
}

const ServiceProperty& ServiceProperty::operator= ( const ServiceProperty& rvalue )
{
	if ( rvalue.NameLength == 0 )
	{
		Empty();
		return *this;
	}

	if ( rvalue.BinaryLength == rvalue.NameLength )
	{
		// Just a flag
		UpdateProperty( NULL );		
	}
	else
	{
		UpdateProperty( &rvalue.Hash[rvalue.NameLength+2] ); // BinaryLength + '-' = 2 bytes
	}

	return *this;
}

void ServiceProperty::Copy( const ServiceProperty& rvalue )
{
	// Copy only the needed bytes from the hash
	memcpy( Hash, rvalue.Hash, rvalue.Length ); // rvalue.Length is at least 1

	Length = rvalue.Length;
	// BinaryLength is already set by copying bytes from the ravlue.Hash
	NameLength = rvalue.NameLength;
	ValueLength = rvalue.ValueLength;
}

bool ServiceProperties::IsDefined( const char * Name )
{
	return (Find(Name) != -1);
}

bool ServiceProperties::Undefine( const char * Name )
{
	int Pos = Find( Name );

	if ( Pos == -1 )
		return false;

	// Ok, we will remove on properties
	NbProperties--;

	for( ; Pos < NbProperties; Pos++ )
	{
		Properties[Pos].Copy( Properties[Pos+1] );
	}

	NotifyChanges();

	return true;
}

int ServiceProperties::Find( const char * Name, bool ReadOnly )
{
	int Pos;
	int NameLen;

	if ( Name == NULL || (NameLen = (int)strlen(Name)) == 0 )
	{
		throw ServicePropertiesException( "Bad parameter" );
	}

	for( Pos = 0; Pos < NbProperties; Pos++ )
	{
		if ( strncasecmp( &Properties[Pos].Hash[1], Name, NameLen ) == 0 )
			return Pos;
	}

	return -1;
}

void ServiceProperties::NotifyChanges()
{
	int i;

	TXTRecordLength = 0;
    for( i = 0; i < NbProperties; i++ )
	{
		TXTRecordLength += Properties[i].Length;
	}
}

const char * ServiceProperties::ExportTXTRecord()
{
	int i;
	int CopyHere;

	if ( TXTRecordLength == 0 )
	{
		return NULL;
	}

	if ( TXTRecordLength > MaxTxtRecordSize )
	{
		throw ServicePropertiesException( "Properties list is too big" );
	}

	for( CopyHere = 0, i = 0; i < NbProperties; i++ )
	{
		if ( Properties[i].Length != 0 )
		{
            memcpy( &TXTRecord[CopyHere], Properties[i].Hash, Properties[i].Length );
			CopyHere += Properties[i].Length;
		}
	}

	return TXTRecord;
}

void ServiceProperties::Empty()
{
	int i;

	TXTRecordLength = 0;
	NbProperties = 0;

	for( i =0; i < MaxProperties; i++ )
	{
		Properties[i].Empty();
	}
}


bool ServiceProperties::ImportTXTRecord( int RecordLength, const char * Record )
{
	int i;
	int NbKeys;
	char KeyName[256];
	char KeyValue[256];
    uint8_t valueLen;
    char *value;
	
	Empty();
	
	NbKeys = TXTRecordGetCount( RecordLength, Record );

	for( i = 0; i < NbKeys; i++ )
	{
		if ( TXTRecordGetItemAtIndex(RecordLength,Record, i, sizeof(KeyName),
			    KeyName, &valueLen, (const void**)&value ) == kDNSServiceErr_NoError )
		{
			// Is it a simple flag ?
			if ( value == NULL )
			{
				(*this)[KeyName] = (char*)NULL;
				continue;
			}
			// Is it an empty value ?
			if ( valueLen == 0 )
			{
				(*this)[KeyName] = "";
				continue;
			}

			// General case
			strncpy( KeyValue, value, valueLen );
			KeyValue[valueLen] = '\0';
			(*this)[KeyName] = KeyValue;
		}
	}

	return true;
}

ServiceProperties::operator char*()
{
	return (char *)ExportTXTRecord();
}

ServiceProperties::operator const char*()
{
	return (const char *)ExportTXTRecord();
}

ServiceProperties::operator unsigned char*()
{
	return (unsigned char *)ExportTXTRecord();
}

ServiceProperties::operator const unsigned char*()
{
	return (const unsigned char *)ExportTXTRecord();
}
