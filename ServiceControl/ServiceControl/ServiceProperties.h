/*! \file
 *  \brief Header of the property classes for services
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef _SERVICE_PROPERTIES_H_
#define _SERVICE_PROPERTIES_H_

#include <System/Config.h>
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
  ServicePropertiesException(const char * Message ) : ServiceException( Message ) {};

  /** @brief Destructor */
  virtual ~ServicePropertiesException() {};

 protected:
  /** @brief Return a human readable exception type */
  virtual const char * GetExceptionType() { return "ServicePropertiesException"; };
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
	ServiceProperty( const char * Name, const char * Value = (const char*)NULL );

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
	virtual bool SetProperty( const char * Name, const char * Value = (const char*)NULL );	// Set a property with a name and optionaly a value

	/*! \brief Change a value for a property that have already a name.
	 *	\param[in] Value an mandatory const c-SimpleString containing the value of the property
	 *  \return a boolean answer if changes are made.
	 *
	 *	This function changes value for a property. If a problem occurs, the
	 *	property will be empty.<BR> See ServiceProperty#SetProperty for possible values.
	 */
	virtual bool UpdateProperty( const char * Value );
	virtual const char * operator= ( const char * rvalue );
	virtual int operator= ( int rvalue );
	virtual const ServiceProperty& operator= ( const ServiceProperty& rvalue );

	const char * GetValue();

	operator char*();
	operator const char*();

protected:
	// Total length of this Property (size included)
	unsigned short Length;

	virtual void Copy( const ServiceProperty& rvalue );

	// Lengths of subfields...
	unsigned char & BinaryLength;
	unsigned char NameLength;
	unsigned char ValueLength;

	// Buffer ready
	// Size consists in 1 byte for length + 255 for content and 1 byte for '\0'
	// because we will use strcpy and co...
	char Hash[256+1];
};

class ServicePropertyNotify : public ServiceProperty
{
	// Our container may access our private values
	friend class ServiceProperties;

public:
	ServicePropertyNotify();
	ServicePropertyNotify(const char * Name, const char * Value = NULL, ServiceProperties * Parent = (ServiceProperties*)NULL );
	virtual ~ServicePropertyNotify();

	virtual bool SetProperty( const char * Name, const char * Value = NULL );
	virtual bool UpdateProperty( const char * Value );
	virtual const char * operator= ( const char * rvalue );
	virtual int operator= ( int rvalue );
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

	// ServiceProperty & operator[]( int Elem );
	ServiceProperty & operator[]( const char * Name );
	// To check if a properties exists
	bool IsDefined( const char * Name );
	bool Undefine( const char * Name );

	int GetNumberOfProperties() { return NbProperties; }

	int GetTXTRecordLength();
	const char * ExportTXTRecord();
	
	operator char*();
	operator const char*();
	operator unsigned char*();
	operator const unsigned char*();

	bool ImportTXTRecord( int RecordLength, const char * Record );

	void NotifyChanges();

	void Empty();

protected:
	enum SizeOfBuffers { MaxTxtRecordSize = 8000 }; // according to the DNS-SD specifications

	virtual int Find( const char * Name, bool ReadOnly = true );

	// Max size of the TXT record field
	int TXTRecordLength;
	char TXTRecord[MaxTxtRecordSize];

	int NbProperties;
	int MaxProperties;
	ServicePropertyNotify * Properties;
};

} // namespace Omiscid

#endif /* _SERVICE_PROPERTIES_H_ */
