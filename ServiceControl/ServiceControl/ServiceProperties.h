/* @file
 * @ingroup ServiceControl
 * @brief Header of the property classes for services
 * @author Dominique Vaufreydaz
 * @author Special thanks to Sébastien Pesnel for debugging and testing
 * @author Special thanks to Julien Letessier for his kind help about DNS-SD
 * @version 1.0
 * @date	2004-2006
 */


#ifndef __SERVICE_PROPERTIES_H__
#define __SERVICE_PROPERTIES_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/SimpleString.h>
#include <ServiceControl/ServicesCommon.h>

namespace Omiscid {

/*! \class ServicePropertiesException
 *	\brief Exception for service properties managment.
 *
 *	This class inherited from ServiceException and just overrides #GetExceptionType.
 */
class ServicePropertiesException  : public ServiceException
{
 public:
  /** @brief Constructor
   * @param Message [in] message for the exception */
  ServicePropertiesException(const SimpleString Message, int Err = UnkownSimpleExceptionCode ) : ServiceException( Message, UnkownSimpleExceptionCode ) {};

  /** @brief Destructor */
  virtual ~ServicePropertiesException();

 protected:
  /** @brief Return a human readable exception type */
  virtual SimpleString GetExceptionType() const { return SimpleString("ServicePropertiesException"); };
};

// A container for service properties, will be defined later in this file
class ServiceProperties;

/*! \class ServiceProperty
 *	\brief A basic property for a DNS-SD service.
 *
 *	This class implements the basic properties for a service as defined in the DNS-SD
 *	documentation. It works using a real memory image of the final data : a 255 bytes buffer
 *	containing both name and optional value for the property. The first byte contains the
 *	full length of this property.
 *
 *	The ServiceProperties class is declared as a friend class of ServiceProperty. Thus,
 *	it can access to the private and protected members.
 */
class ServiceProperty
{
public:
	/*! \brief Default constructor.
	 *
	 *	The default constructor constructs an empty property (see ServiceProperty#Empty).
	*/
	ServiceProperty();

	/*! \brief Alternate constructor.
	 *	\param[in] Name a const c-SimpleString containing the property name.
	 *	\param[in] Value an optional const c-SimpleString containing the value of the property
	 *	\throw #ServicePropertiesException The value of the error message sould be "Bad parameter(s) for the ServiceProperty constructor"
	 *
	 *	This constructor takes a name and optionaly a value for the new constructed property. See ServiceProperty#SetProperty.
	*/
	ServiceProperty( const SimpleString Name, const SimpleString Value = SimpleString::EmptyString() );

	virtual ~ServiceProperty();

	/*! \brief Empty a property.
	*
	*	This function empty a property. The #Length is set to 1 (just the byte containing
	*	the length of the property), the intinsect #BinaryLength of the property (the only byte) is set to 0
	*	as the #NameLength and the #ValueLength.
	*
	*	In debug mode, we also full the Hash with 0.
	*/
	void Empty();

	/*! \brief Set name and optionaly value for a property.
	 *	\param[in] Name a const c-SimpleString containing the property name.
	 *	\param[in] Value an optional const c-SimpleString containing the value of the property
	 *  \return a boolean answer if changes are made.
	 *
	 *	This function changes name and optionaly value for a property. If a problem occurs, the
	 *	property remains unchanged. You can define property as follow:
	 *  \li \c if #Name equals "Property" and #Value is NULL, we will have a flag : "Property".
	 *  \li \c if #Name equals "Property" and #Value is "", we will have an empty property : "Property=".
	 *  \li \c if #Name equals "Property" and #Value is "something", we will have : "Property=something".
	 */
	virtual bool SetProperty( const SimpleString Name, const SimpleString Value = SimpleString::EmptyString() );	// Set a property with a name and optionaly a value

	/*! \brief Change a value for a property that have already a name.
	 *	\param[in] Value an mandatory const c-SimpleString containing the value of the property
	 *  \return a boolean answer if changes are made.
	 *
	 *	This function changes value for a property. If a problem occurs, the
	 *	property will be empty.<BR> See ServiceProperty#SetProperty for possible values.
	 */
	virtual bool UpdateProperty( const SimpleString Value );
	virtual const SimpleString operator= ( const SimpleString rvalue );
	virtual const ServiceProperty& operator= ( const ServiceProperty& rvalue );

	const SimpleString GetValue();
	const SimpleString GetName();

protected:
	// Total length of this Property (size included)
	unsigned int Length;

	virtual void Copy( const ServiceProperty& rvalue );

	// Name and value
	SimpleString Name;
	SimpleString Value;
};

class ServicePropertyNotify : public ServiceProperty
{
	// Our container may access our private values
	friend class ServiceProperties;

public:
	ServicePropertyNotify();
	ServicePropertyNotify(const SimpleString Name, const SimpleString Value = SimpleString::EmptyString(), ServiceProperties * Parent = (ServiceProperties*)NULL );
	virtual ~ServicePropertyNotify();

	virtual bool SetProperty( const SimpleString Name, const SimpleString Value = SimpleString::EmptyString() );
	virtual bool UpdateProperty( const SimpleString Value );
	virtual const SimpleString operator= ( const SimpleString rvalue );
	virtual const ServiceProperty& operator= ( const ServiceProperty& rvalue );
	virtual const ServicePropertyNotify& operator= ( const ServicePropertyNotify& rvalue );

	void SetNotify( ServiceProperties * Parent );

protected:
	virtual void Copy( const ServicePropertyNotify& rvalue );

	ServiceProperties * Container;
};

class ServiceProperties
{
public:
	// At first, we have at least 8 possible properties, by default
	ServiceProperties( int InitialNumberOfProperties = 8 );
	virtual ~ServiceProperties();

	ServiceProperty & GetProperty( int Elem );
	ServiceProperty & operator[]( const SimpleString Name );

	// To check if a properties exists
	bool IsDefined( const SimpleString Name );
	bool Undefine( const SimpleString Name );

	int GetNumberOfProperties() { return NbProperties; }

	int GetTXTRecordLength() const;
	const unsigned char * ExportTXTRecord() const;

	operator char*();
	operator const char*();
	operator unsigned char*();
	operator const unsigned char*();

	bool TxtRecordIsFull() const;
	bool ImportTXTRecord( int RecordLength, const unsigned char * Record );

	void NotifyChanges();

	void Empty();

	const ServiceProperties& operator=(const ServiceProperties& ToCopy );

protected:
	// according to the DNS-SD specifications, can be up to 8000 bytes
	// but not on multicast usage
#ifdef OMISCID_USE_MDNS
	enum SizeOfBuffers { MaxTxtRecordSize = 1024 };
#else
#ifdef OMISCID_USE_AVAHI
	// For avahi, there is a cookie set in TXT record, so let's say that
	// we have 200 characters less
	enum SizeOfBuffers { MaxTxtRecordSize = 824 };
#endif
#endif

	virtual int Find( const SimpleString Name, bool ReadOnly = true );

	// Max size of the TXT record field
	int TXTRecordLength;
	unsigned char TXTRecord[MaxTxtRecordSize];

	int NbProperties;
	int MaxProperties;
	ServicePropertyNotify * Properties;
};

} // namespace Omiscid

#endif // __SERVICE_PROPERTIES_H__
