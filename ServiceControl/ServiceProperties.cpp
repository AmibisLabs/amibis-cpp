/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
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
	Name.Empty();
	Value.Empty();
}

/*! \brief Default constructor.
 *
 *	The default constructor constructs an empty property (see ServiceProperty#Empty).
 */
ServiceProperty::ServiceProperty()
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

	if ( UpdateProperty( Value ) == false )
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

	Length = Name.GetLength()+1+Value.GetLength() + 1;

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

bool ServiceProperties::TxtRecordIsFull()
{
	if ( TXTRecordLength > MaxTxtRecordSize )
		return true;

	return false;
}

const char * ServiceProperties::ExportTXTRecord()
{
	int i;
	int CopyHere;
	unsigned char * SetPropertyLength;
	SimpleString    TmpProperty;

	if ( TXTRecordLength == 0 )
	{
		return NULL;
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
			*SetPropertyLength = Properties[i].Length-1;
            
			// Let's generate and copy the propertie
			TmpProperty  = Properties[i].Name;
			TmpProperty += "=";
			if ( Properties[i].Value.GetLength() != 0 )
			{
				TmpProperty += Properties[i].Value;
			}
			// Copy after the length the value of the property
			memcpy( &TXTRecord[CopyHere+1], TmpProperty.GetStr(), Properties[i].Length-1 );
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
			if ( value == NULL || valueLen == 0 )
			{
				(*this)[KeyName] = SimpleString::EmptyString;
				continue;
			}

			// General case
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
