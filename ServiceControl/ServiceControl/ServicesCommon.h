/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef _SERVICES_COMMON_H_
#define _SERVICES_COMMON_H_

#include <System/Config.h>
#include <System/Socket.h>
#include <System/SimpleString.h>

#include <stdio.h>
#include <stdlib.h>


#include <dns_sd.h>


#include <System/SimpleException.h>

namespace Omiscid {

/*! \class ServiceException
 *  \brief The mother class of all exceptions of the PRIMA Service Package.
 *
 *  This class is the mother class of all exceptions raised by the PRIMA Service
 *  package. All derived classes must define their own exception type (const char*)
 *  by rewriting the virtual function ServiceException#GetExceptionType.
 */
	class ServiceException : public SimpleException
{
 public:
  /** \brief Constructor 
   * \param m [in] the message for the exception
   */
  ServiceException(const char* m);

  /** \brief Copy Constructor 
   * \param ExceptionToCopy [in] the exception to copy
   */
  ServiceException(const ServiceException& ExceptionToCopy);

  /** \brief Destructor */
  virtual ~ServiceException();  

  /** \brief Return a human readable exception type */
  virtual const char * GetExceptionType() { return "ServiceException"; };
};


/*! \class CommonServiceValues
 *	\brief The mother class of all Service classes of the PRIMA Service package.
 *
 *	This class is the mother class of all Service classes. It defines common values
 *	defined in DNS-SD documentation like fields length.
 */
class CommonServiceValues
{
public:
		/*! \enum TransportProtocol
		*	\brief Defines the transport protocol used by the service.
		*
		*	This enum is used to define the control protocol used by the service. Supported protocols
		*	are TCP and UDP.
		*/
		enum TransportProtocol{
			UNKNOWN = 0,		/*!< set when the transport protocol is undefined or unsupported. */
			TCP = Socket::UDP,  /*!< set when the service uses TCP. It is defined in Socket.h using the usual SOCK_STEAM value. */
			UDP = Socket::TCP	/*!< set when the service uses UCP. It is defined in Socket.h using the usual SOCK_DGRAM value. */
		};

		/*! \enum SizeOfBuffers
		*	\brief Defines length of service information.
		*
		*	According to the DNS-SD documentation, the maximal length of all fields for a service names are defined
		*	as expressed in this enum.
		*/
		enum SizeOfBuffers {
			ServiceField = 64,	/*!< the service name length */
			ServiceNameLength = (3*ServiceField + sizeof("_tcp") + kDNSServiceMaxDomainName + 4), /*!< the complete unique name length of a service like "Doms._printer._tcp.local." */
			RegtypeLength = ServiceField + sizeof("_tcp")/*!< the length of the protocol and the transport protocol for a service, i.e "_printer._tcp" */
		};

		static char * OmiscidServiceDnsSdType;
		static const char * DefaultDomain;
};

} // namespace Omiscid

#endif /* _SERVICES_COMMON_H_ */
