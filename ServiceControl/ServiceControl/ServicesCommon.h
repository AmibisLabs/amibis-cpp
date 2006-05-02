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

#ifdef WIN32
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#include <System/Socket.h>
#include <System/SimpleString.h>

#include <dns_sd.h>

/*! \def FUNCTION_CALL_TYPE
 *	\brief DNS-SD callbacks type (WIN32 specific).
 *
 *	As WIN32 plateform supports many function call types, we must use the right
 *	one for DNS-SD callbacks. As defined in DNS-SD documentation, under WIN32 plateforms
 *	the callback type is __stdcall.	On other plateform, this define is empty.
 */
/*! \def strcasecmp
 *	\brief Wrapper for the unix strcasecmp function (WIN32 only).
 *
 */
/*! \def strncasecmp
 *	\brief Wrapper for the unix strncasecmp function (WIN32 only).
 *
 */
#ifdef WIN32
	#define FUNCTION_CALL_TYPE __stdcall
	/*! \def DEBUG
	 *	\brief Define the gcc like debug symbol (WIN32 only).
     *
	 *	In order to have portable DEBUG support, we need to define a common
	 *	debug symbol. We choose to use the debug symbol used by gcc : DEBUG.
	 */
	#ifdef _DEBUG
		#define DEBUG
	#endif
	#define strcasecmp stricmp
	#define strncasecmp strnicmp
#else
	#define FUNCTION_CALL_TYPE
	#include <sys/socket.h>
	#define strcasecmp strcasecmp
	#define strncasecmp strncasecmp

	typedef int SOCKET;
	#define SOCKET_ERROR (-1)
#endif

#include <System/SimpleException.h>

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
		typedef enum {
			UNKNOWN = 0,		/*!< set when the transport protocol is undefined or unsupported. */
			TCP = Socket::UDP,  /*!< set when the service uses TCP. It is defined in Socket.h using the usual SOCK_STEAM value. */
			UDP = Socket::TCP	/*!< set when the service uses UCP. It is defined in Socket.h using the usual SOCK_DGRAM value. */
		} TransportProtocol;

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
};

#endif /* _SERVICES_COMMON_H_ */
