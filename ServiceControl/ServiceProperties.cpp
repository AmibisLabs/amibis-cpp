/*! @file
 *  @brief Header of the common classes and values for the OMiSCID service package
 *  @author Dominique Vaufreydaz
 *  @author Special thanks to Sébastien Pesnel for debugging and testing
 *  @author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date	2004-2006
 */

#include <ServiceControl/ServiceProperties.h>

#ifdef WIN32
#pragma warning(disable : 4312)
#endif	// WIN32

using namespace Omiscid;

ServicePropertiesException::~ServicePropertiesException()
{
}

/*! @brief Empty a property.
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
	Name.Empty();
	Value.Empty();
}

/*! @brief Default constructor.
 *
 *	The default constructor constructs an empty property (see ServiceProperty#Empty).
 */
ServiceProperty::ServiceProperty()
{
	// Just call the empty function
	Empty();
}

/*! @brief Alternate constructor.
 *	@param[in] Name a const c-SimpleString containing the property name.
 *	@param[in] Value an optional const c-SimpleString containing the value of the property
 *	\throw ServicePropertiesException The value of the error message sould be "Bad parameter(s) for the ServiceProperty constructor"
 *
 *	This constructor takes a name and optionaly a value for the new constructed property. See ServiceProperty#SetProperty.
 */
ServiceProperty::ServiceProperty( const SimpleString Name, const SimpleString Value )
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

/*! @brief Set name and optionaly value for a property.
 *	@param[in] Name a const c-SimpleString containing the property name.
 *	@param[in] Value an optional const c-SimpleString containing the value of the property
 *  @return a boolean answer if changes are made.
 *
 *	This function changes name and optionaly value for a property. If a problem occurs, the
 *	property will be empty. You can define property as follow:
 *  \li \c if #Name equals "Property" and #Value is NULL, we will have a flag : "Property".
 *  \li \c if #Name equals "Property" and #Value is "", we will have an empty property : "Property=".
 *  \li \c if #Name equals "Property" and #Value is "something", we will have : "Property=something".
 */
bool ServiceProperty::SetProperty( const SimpleString eName, const SimpleString eValue )
{
	// First of all, empty the property
	Empty();

	// Empty names are illegals
	// Name + '=' + value must be 255 long at max !!!
	if ( eName.GetLength() == 0 || (eName.GetLength()+1+eValue.GetLength()) > 255 )
	{
		return false;
	}

	// Set Name
	Name = eName;

	// Set length to the Value of the name
	Length = Name.GetLength() + 1;

	if ( UpdateProperty( eValue ) == false )
	{
		Empty();
	}

	return true;
}

bool ServiceProperty::UpdateProperty( const SimpleString eValue )
{
	if ( Name.GetLength() == 0 )
	{
		return false;
	}

	Value = eValue;

	Length = Name.GetLength() + 1 + Value.GetLength() + 1;

	return true;
}

const SimpleString ServiceProperty::operator= ( const SimpleString rvalue )
{
	UpdateProperty( rvalue );
	return rvalue;
}

const SimpleString ServiceProperty::GetValue()
{
	if ( Value.GetLength() == 0 )
		return SimpleString::EmptyString;

	return Value;
}

const SimpleString ServiceProperty::GetName()
{
	if ( Name.GetLength() == 0 )
		return SimpleString::EmptyString;

	return Name;
}

ServicePropertyNotify::ServicePropertyNotify() : ServiceProperty(), Container(NULL)
{
}

ServicePropertyNotify::ServicePropertyNotify( const SimpleString eName, const SimpleString eValue, ServiceProperties * Parent )
: ServiceProperty( eName, eValue ), Container(Parent)
{
}

ServicePropertyNotify::~ServicePropertyNotify()
{}

void ServicePropertyNotify::SetNotify( ServiceProperties * Parent )
{
	Container = Parent;
}

bool ServicePropertyNotify::SetProperty( const SimpleString eName, const SimpleString eValue )
{
	bool ret = ServiceProperty::SetProperty( eName, eValue );
	if ( Container && ret )
	{
		Container->NotifyChanges();
	}

	return ret;
}


bool ServicePropertyNotify::UpdateProperty( const SimpleString eValue )
{
	bool ret = ServiceProperty::UpdateProperty( eValue );
	if ( Container && ret )
	{
		Container->NotifyChanges();
	}

	return ret;
}

const SimpleString ServicePropertyNotify::operator= ( const SimpleString rvalue )
{
	const SimpleString ret = ServiceProperty::operator=( rvalue );
	if ( Container && ret.GetLength() != 0 )
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

	Properties = new OMISCID_TLM ServicePropertyNotify[i];

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

int ServiceProperties::GetTXTRecordLength() const
{
	return TXTRecordLength;
}

ServiceProperty & ServiceProperties::GetProperty( int Elem )
{
	if ( Elem < 0 || Elem >= this->NbProperties )
	{
		// We are out of band !
		throw ServicePropertiesException( "Out of band" );
	}

	return Properties[Elem];
}

ServiceProperty& ServiceProperties::operator[]( const SimpleString Name )
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

		ServicePropertyNotify * tmpProperties = new OMISCID_TLM ServicePropertyNotify[i];

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

	Properties[NbProperties].SetProperty( Name, SimpleString::EmptyString );

	NbProperties++;

	NotifyChanges();

	return Properties[NbProperties-1];
}

const ServiceProperty& ServiceProperty::operator= ( const ServiceProperty& rvalue )
{
	if ( rvalue.Name.GetLength() == 0 )
	{
		Empty();
		return *this;
	}

	// Copy the ServiceProperty
	Copy(rvalue);

	return *this;
}

void ServiceProperty::Copy( const ServiceProperty& rvalue )
{
	// Copy only the needed bytes from the hash
	Name = rvalue.Name;
	Value = rvalue.Value;
	Length = rvalue.Length;
}

bool ServiceProperties::IsDefined( const SimpleString Name )
{
	return (Find(Name) != -1);
}

bool ServiceProperties::Undefine( const SimpleString Name )
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

int ServiceProperties::Find( const SimpleString eName, bool ReadOnly )
{
	int Pos;

	if ( eName.IsEmpty() )
	{
		throw ServicePropertiesException( "Bad parameter" );
	}

	for( Pos = 0; Pos < NbProperties; Pos++ )
	{
		if ( strcasecmp( eName.GetStr(), Properties[Pos].Name.GetStr() ) == 0 )
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

bool ServiceProperties::TxtRecordIsFull() const
{
	if ( TXTRecordLength > MaxTxtRecordSize )
		return true;

	return false;
}

const unsigned char * ServiceProperties::ExportTXTRecord() const
{
	int i;
	int CopyHere;
	unsigned char * SetPropertyLength;
	SimpleString	TmpProperty;

	if ( TXTRecordLength == 0 )
	{
		return (unsigned char *)NULL;
	}

	if ( TxtRecordIsFull() )
	{
		throw ServicePropertiesException( "Properties list is too big" );
	}

	for( CopyHere = 0, i = 0; i < NbProperties; i++ )
	{
		if ( Properties[i].Length != 0 && Properties[i].Length < 255 )
		{
			// Set the property length in the buffer
			SetPropertyLength  = (unsigned char*)&TXTRecord[CopyHere];
			*SetPropertyLength = (unsigned char)Properties[i].Length-1;

			// Let's generate and copy the propertie
			TmpProperty  = Properties[i].Name;
			TmpProperty += "=";
			if ( Properties[i].Value.GetLength() != 0 )
			{
				TmpProperty += Properties[i].Value;
			}
			// Copy after the length the value of the property
			memcpy( (char*)&TXTRecord[CopyHere+1], TmpProperty.GetStr(), Properties[i].Length-1 );
			CopyHere += Properties[i].Length;
		}
	}

	((char*)TXTRecord)[TXTRecordLength] = '\0';

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


bool ServiceProperties::ImportTXTRecord( int RecordLength, const unsigned char * Record )
{

#ifdef OMISCID_USE_MDNS
	int i;
	int NbKeys;
	char KeyName[256];
	char KeyValue[256];
	uint8_t valueLen;
	unsigned char *value;
	unsigned char **pvalue = &value; // in order to prevent dumb warning from gcc >= 4.1.2...

	Empty();

	NbKeys = TXTRecordGetCount( (uint16_t)RecordLength, Record );

	for( i = 0; i < NbKeys; i++ )
	{
		if ( TXTRecordGetItemAtIndex((uint16_t)RecordLength,Record, (uint16_t)i, (uint16_t)sizeof(KeyName),
				KeyName, &valueLen, (const void**)pvalue ) == kDNSServiceErr_NoError )
		{
			// Is it a simple flag ?
			if ( value == NULL || valueLen == 0 )
			{
				(*this)[KeyName] = SimpleString::EmptyString;
				continue;
			}

			// General case
			strncpy( KeyValue, (const char*)value, valueLen );
			KeyValue[valueLen] = '\0';
			(*this)[KeyName] = KeyValue;
		}
	}
#else
#ifdef OMISCID_USE_AVAHI
	char * KeyName = (char*)NULL;
	char * KeyValue = (char*)NULL;

	AvahiStringList * TxtRecordStringList = (AvahiStringList *)NULL;
	AvahiStringList * PosIntoTxtRecord;

	// Empty properties
	Empty();

	// parse file to construct avahi string list
	if ( avahi_string_list_parse( (const void*)Record, (size_t)RecordLength, &TxtRecordStringList ) < 0 )
	{
		// Could not parse string
		return false;
	}

	for( PosIntoTxtRecord = TxtRecordStringList; PosIntoTxtRecord != (AvahiStringList *)NULL; PosIntoTxtRecord = avahi_string_list_get_next(PosIntoTxtRecord) )
	{
		// Get pair
		if ( avahi_string_list_get_pair(PosIntoTxtRecord, &KeyName, &KeyValue, (size_t*)NULL) < 0 )
		{
			continue;
		}

		// Set this properties
		(*this)[KeyName] = KeyValue;

		// Free KeyName
		avahi_free( KeyName );
		KeyName = (char*)NULL;

		// Free KeyValue if any
		if ( KeyValue != (char*)NULL )
		{
			avahi_free( KeyValue );
			KeyValue = (char*)NULL;
		}
	}

	// Free list
	avahi_string_list_free( TxtRecordStringList );

#endif
#endif

	// Here everything goes fine
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

const ServiceProperties& ServiceProperties::operator=(const ServiceProperties& ToCopy )
{
	ImportTXTRecord( ToCopy.GetTXTRecordLength(), ToCopy.ExportTXTRecord() );
	return *this;
}
